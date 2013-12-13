<?php
/**
 * Implements hook_field_formatter_info().
 */
function pdf_field_formatter_info() {
  return array(
    'pdf_default' => array(
      'label' => t('PDF: Default viewer of PDF.js'),
      'field types' => array('file'),
      'settings'=>array('keep_pdfjs' => TRUE),
    ),
    'pdf_thumbnail' => array(
      'label' => t('PDF: Display the first page'),
      'field types' => array('file'),
    ),
    'pdf_pages' => array(
      'label' => t('PDF: Continuous scroll'),
      'field types' => array('file'),
      'settings'=>array('scale' => 1),
    ),
  );
}

function pdf_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  if ($display['type'] == 'pdf_default') {
    $element['keep_pdfjs'] = array(
      '#type' => 'checkbox',
      '#title' => t('Always use pdf.js'),
      '#default_value' => $settings['keep_pdfjs'],
      '#description' => t('Pdf.js will always be used even if the browser has PDF plugin. Uncheck it if you just want to use pdf.js when the browser doesn\'t have PDF plugin.'),
    );
  }
  if ($display['type'] == 'pdf_pages') {
    $element['scale'] = array(
      '#type' => 'textfield',
      '#title' => t('Set the scale of PDF pages'),
      '#default_value' => $settings['scale'],
      //'#description' => t(''),
    );
  }
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function pdf_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  if ($display['type'] == 'pdf_default') {
    return t('Use pdf.js even users have PDF reader plugin: @keep_pdfjs', array('@keep_pdfjs' => $settings['keep_pdfjs'] ? t('Yes') : t('No')));
  }
  if ($display['type'] == 'pdf_pages') {
    return t('Scale: @scale', array('@scale' => $settings['scale']));
  }
  return '';
}

/**
 * Implements hook_field_formatter_view().
 */
function pdf_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  $settings = $display['settings'];
  switch ($display['type']) {
    case 'pdf_default':
      foreach ($items as $delta => $item) {
        if ( strpos($item['filemime'], 'pdf' ) ) {
          $element[$delta] = array(
            '#theme' => 'pdf_formatter_default',
            '#file' => (object)$item,
            '#keep_pdfjs' => $settings['keep_pdfjs'],
          );
        }
      }
      break;
    case 'pdf_thumbnail':
      foreach ($items as $delta => $item) {
        if ( strpos($item['filemime'], 'pdf' ) ) {
          $element[$delta] = array(
            '#theme' => 'pdf_formatter_thumbnail',
            '#file' => (object)$item,
          );
        }
      }
      break;
    case 'pdf_pages':
      foreach ($items as $delta => $item) {
        if ( strpos($item['filemime'], 'pdf' ) ) {
          $element[$delta] = array(
            '#theme' => 'pdf_formatter_pages',
            '#file' => (object)$item,
            '#scale' => $settings['scale'],
          );
        }
      }
      break;
  }
  return $element;
}

function theme_pdf_formatter_default($variables) {
  global $base_path;
  $file_url = file_create_url($variables['file']->uri);
  $module_path = drupal_get_path('module', 'pdf');
  $library_path = libraries_get_path('pdf.js');
  $iframe_src = $base_path . $library_path . '/web/viewer.html?file=' . $file_url;
  $force_pdfjs = $variables['keep_pdfjs'];
  $content = array(
    '#type' => 'html_tag',
    '#tag' => 'iframe',
    '#value' => $file_url,
    '#attributes' => array(
      'class' => array('pdf'),
      'webkitallowfullscreen' => '',
      'mozallowfullscreen' => '',
      'allowfullscreen' => '',
      'frameborder' => 'no',
      'height' => '600px',
      'src' => $force_pdfjs ? $iframe_src : $file_url,
      //'data-src' => $iframe_src,
    ),
  );
  if ($force_pdfjs != TRUE) {
    drupal_add_js($module_path . '/js/acrobat_detection.js');
  }

  drupal_add_css($module_path . '/css/pdf.css');

  return render($content);
}

function theme_pdf_formatter_thumbnail($variables) {
  $file_url = file_create_url($variables['file']->uri);
  drupal_add_js(libraries_get_path('pdf.js') . '/build/pdf.js');

  $worker_loader = url($GLOBALS['base_url'] . '/' . libraries_get_path('pdf.js') . '/build/pdf.worker.js');
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

  drupal_add_js($js, array('type' => 'inline'));
  $output = '<canvas id="the-canvas" style="width:99%; border:1px solid black;"/>';
  return $output;
}

function theme_pdf_formatter_pages($variables) {
  $scale = $variables['scale'];
  $file_url = file_create_url($variables['file']->uri);
  $filename = $variables['file']->filename;
  $fid = $variables['file']->fid;
  drupal_add_css(drupal_get_path('module', 'pdf') . '/css/TextLayer.css');
  drupal_add_css(drupal_get_path('module', 'pdf') . '/css/pdf.css');
  drupal_add_js(libraries_get_path('pdf.js') . '/build/pdf.js');

  $worker_loader = url($GLOBALS['base_url'] . '/' . libraries_get_path('pdf.js') . '/build/pdf.worker.js');
  $js = "PDFJS.workerSrc = '$worker_loader';";
  drupal_add_js($js, array('type' => 'inline'));
  drupal_add_js(drupal_get_path('module', 'pdf') . '/js/TextLayerBuilder.js');
  drupal_add_js(drupal_get_path('module', 'pdf') . '/js/PdfListView.js');
  drupal_add_js(drupal_get_path('module', 'pdf') . '/js/pdf.js');
  $link = array(
    'text' => $filename,
    'path' => $file_url,
    'options' => array(
      'attributes' => array(),
      'html' => FALSE,
     ),
  );
  $content = format_string('<div class="pdf" id="viewer fid-@fid" file="@file" scale="@scale">!link</div>', array('@fid' => $fid, '@file' => $file_url, '@scale' => $scale, '!link' => t('Download: ') . theme_link($link)));
  return $content;
}

