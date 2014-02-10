(function ($) {
  Drupal.behaviors.pdf = {
    attach: function(context, settings) {
      $('.pdf').each(function(){
        var file = $(this).attr('file');
        var scale = $(this).attr('scale');
        $(this).append('<div id="pdfContainer" class="pdf-content"/>');

        function loadPdf(pdfPath) {
            var pdf = PDFJS.getDocument(pdfPath);
            pdf.then(renderPdf);
        }

        function renderPdf(pdf) {
            for (var i = 1; i <= pdf.pdfInfo.numPages; i++) {
              pdf.getPage(i).then(renderPage);
            }
        }

        function renderPage(page) {
            var viewport = page.getViewport(scale);
            var $canvas = jQuery("<canvas></canvas>");

            //Set the canvas height and width to the height and width of the viewport
            var canvas = $canvas.get(0);
            var context = canvas.getContext("2d");
            canvas.height = viewport.height;
            canvas.width = viewport.width;

            //Append the canvas to the pdf container div
            var $pdfContainer = $('#pdfContainer');
            //$pdfContainer.append($canvas);

            var canvasOffset = $canvas.offset();
            var $textLayerDiv = jQuery("<div />")
                .addClass("textLayer")
                .css("height", viewport.height + "px")
                .css("width", viewport.width + "px");

            //The following few lines of code set up scaling on the context if we are on a HiDPI display
            var outputScale = getOutputScale(context);
            if (outputScale.scaled) {
                var cssScale = 'scale(' + (1 / outputScale.sx) + ', ' +
                    (1 / outputScale.sy) + ')';
                CustomStyle.setProp('transform', canvas, cssScale);
                CustomStyle.setProp('transformOrigin', canvas, '0% 0%');

                if ($textLayerDiv.get(0)) {
                    CustomStyle.setProp('transform', $textLayerDiv.get(0), cssScale);
                    CustomStyle.setProp('transformOrigin', $textLayerDiv.get(0), '0% 0%');
                }
            }

            context._scaleX = outputScale.sx;
            context._scaleY = outputScale.sy;
            if (outputScale.scaled) {
                context.scale(outputScale.sx, outputScale.sy);
            }

            console.log(page);
            $pdfContainer.append('<div id="pdf-page"/>');
            $pdfContainer.find('div#pdf-page:last-child').append($canvas).append($textLayerDiv);

            page.getTextContent().then(function (textContent) {

                var textLayer = new TextLayerBuilder({
                    textLayerDiv: $textLayerDiv.get(0),
                    pageIndex: 0
                });

                textLayer.setTextContent(textContent);

                var renderContext = {
                    canvasContext: context,
                    viewport: viewport,
                    textLayer: textLayer
                };

                page.render(renderContext);
            });
        }

        loadPdf(file);

      });
    }
  };
})(jQuery);
