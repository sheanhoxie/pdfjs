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

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {

    $library = libraries_load('pdf.js');
    if ($library['loaded'] == FALSE) {
      drupal_set_message($library['error message'], 'error');
      return t('Please download and install ') . \Drupal::l( $library['name'], Url::fromUri($library['download url'])) . '!';

    }

    $elements = array();
    foreach ($items as $delta => $item) {
      $filename = $item->entity->getFilename();
      if ($item->isDisplayed() && $item->entity && strpos($filename, 'pdf' ) ) {
        $file_url = file_create_url($item->entity->getFileUri());
        $worker_loader = file_create_url(libraries_get_path('pdf.js') . '/build/pdf.worker.js');
        $js = "PDFJS.workerSrc = '$worker_loader';";
        $js .= "'use strict';
          PDFJS.getDocument('$file_url').then(function(pdf) {
            pdf.getPage(1).then(function(page) {
              var scale = 1;
              var viewport = page.getViewport(scale);
              var canvas = document.getElementById('the-canvas');
              var context = canvas.getContext('2d');
              canvas.height = viewport.height;
              canvas.width = viewport.width;
              var renderContext = {
                canvasContext: context,
                viewport: viewport
              };
              page.render(renderContext);
            });
          });
        ";
        $output = '<canvas id="the-canvas" style="width:99%; border:1px solid black;"/><script type="text/javascript">' . $js . '</script>';

        $elements[$delta] = array('#markup' => $output);
      }
    }

    return $elements;
  }
}
