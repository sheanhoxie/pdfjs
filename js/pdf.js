(function ($) {
  Drupal.behaviors.pdf = {
    renderPages: function(target, file) {
      PDFJS.getDocument(file).then(function(pdf) {
        //console.log(pdf);
        for (var i=1; i<=pdf.numPages; i++) {
          var canvas = $('<canvas/>', {'id': 'page'+i});
          target.append($('<div/>', {'class': 'page', 'id': 'pageContainer'+i}));
          $('#pageContainer'+i).append(canvas);
/*
          var textLayerDiv = null;
          if (!PDFJS.disableTextLayer) {
            textLayerDiv = document.createElement('div');
            textLayerDiv.className = 'textLayer';
            $('#pageContainer'+i).append($(textLayerDiv));
          }
*/        
          pdf.getPage(i).then(function(page) {
            var scale = 1;
            var viewport = page.getViewport(scale);
            var canvas = target.find('canvas')[page.pageNumber-1];
            var context = canvas.getContext('2d');
            canvas.height = viewport.height;
            canvas.width = viewport.width;
/*
            textLayerDiv = target.find('div')[page.pageNumber-1]
            textLayerDiv.style.width = canvas.width + 'px';
            textLayerDiv.style.height = canvas.height + 'px';
            var textLayer = this.textLayer = textLayerDiv ? new TextLayerBuilder(textLayerDiv, page.pageNumber - 1) : null;
*/
            var renderContext = {
              canvasContext: context,
              viewport: viewport
            };
            page.render(renderContext);//, textLayer);
          });
        }
      });
    },

    attach: function(context, settings) {
      $('.pdf').each(function(){
        var file = $(this).attr('file');
        //$(this).html('');
        Drupal.behaviors.pdf.renderPages($(this), file);
      });
    }
  };
})(jQuery);
