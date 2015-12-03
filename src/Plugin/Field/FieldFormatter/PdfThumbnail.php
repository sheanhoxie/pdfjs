<?php /**
 * @file
 * Contains \Drupal\pdf\Plugin\Field\FieldFormatter\PdfThumbnail.
 */
namespace Drupal\pdf\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Url;

/**
 * @FieldFormatter(
 *  id = "pdf_thumbnail",
 *  label = @Translation("PDF: Display the first page"),
 *  description = @Translation("Display the first page of the PDF file."),
 *  field_types = {"file"}
 * )
 */
class PdfThumbnail extends FormatterBase {
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = array();
    foreach ($items as $delta => $item) {
      $filename = $item->entity->getFilename();
      if ($item->isDisplayed() && $item->entity && strpos($filename, 'pdf' ) ) {
        $file_url = file_create_url($item->entity->getFileUri());
        $html = array(
          '#type' => 'html_tag',
          '#tag' => 'canvas',
          //'#value' => ,
          '#attributes' => array(
            'class' => array('pdf-thumbnail', 'pdf-canvas'),
            'id' => array('pdf-thumbnail-' . $delta),
            'file' => $file_url
          ),
        );
        $elements[$delta] = array(
          '#markup' => \Drupal::service('renderer')->render($html),
        );
      }
    }
    $elements['#attached']['library'][] = 'pdf/drupal.pdf';
    return $elements;
  }
}
