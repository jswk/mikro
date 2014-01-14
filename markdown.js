var markdownpdf = require("markdown-pdf")
  , fs = require("fs")

markdownpdf("README.md", {
	cssPath : "./css.css",
	paperBorder : "2cm"
}, function (er, pdfPath) {
    if (er) return console.error(er)
    
    var is = fs.createReadStream(pdfPath);
    var os = fs.createWriteStream('README.pdf');

    is.pipe(os);
    is.on('end',function() {
        fs.unlinkSync(pdfPath);
        console.log('done');
    });
})