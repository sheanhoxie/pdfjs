<?php /**
 * @file
 * Contains \Drupal\pdf\Plugin\Field\FieldFormatter\PdfPages.
 */

namespace Drupal\pdf\Plugin\Field\FieldFormatter;


use Drupal\Core\Field\FormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;

/**
 * @FieldFormatter(
 *  id = "pdf_pages",
 *  label = @Translation("PDF: Continuous scroll"),
 *  description = @Translation("Don&#039;t use this to display big PDF file."),
 *  field_types = {"file"}
 * )
 */
class PdfPages extends FormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return array(
      'scale' => '',
    ) + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $elements['scale'] = array(
      '#type' => 'textfield',
      '#title' => t('Set the scale of PDF pages'),
      '#default_value' => $this->getSetting('scale'),
    );
    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = array();
    $scale = $this->getSetting('scale');
    if (empty($scale)) {
      $summary[] = $this->t('No settings');
    }
    else {
      $summary[] = t('Scale: @scale', array('@scale' => $scale));
    }

    return $summary;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {

    $library = libraries_load('pdf.js', 'viewer');
    if ($library['loaded'] == FALSE) {
      drupal_set_message($library['error message'], 'error');
      return t('Please download and install ') . \Drupal::l( $library['name'], Url::fromUri($library['download url'])) . '!';

    }

    $worker_loader = file_create_url(libraries_get_path('pdf.js') . '/build/pdf.worker.js');
    $js = '<script type="text/javascript">PDFJS.workerSrc = ' . $worker_loader . ';</script>';
    $content['#attached']['library'][] = 'pdf/pdf.js';
    $content['#attached']['library'][] = 'pdf/css';
    drupal_process_attached($content);

    $elements = array();
    foreach ($items as $delta => $item) {
      $filename = $item->entity->getFilename();
      if ($item->isDisplayed() && $item->entity && strpos($filename, 'pdf') ) {
        $scale = $this->getSetting('scale');
        $file_url = file_create_url($item->entity->getFileUri());
        $fid = $delta;

        $link = \Drupal::l($filename, Url::fromUri($file_url));
        $content = format_string('<div class="pdf" id="viewer fid-@fid" file="@file" scale="@scale">!link</div>', array('@fid' => $fid, '@file' => $file_url, '@scale' => $scale, '!link' => t('Download: ') . $link));
        $elements[$delta] = array('#markup' => $content . $js);
      }
    }

    return $elements;
  }

}
