(function () {
	var jquery_el = document.createElement("script");
	jquery_el.type = "text/javascript";
	jquery_el.src = "http://code.jquery.com/jquery-2.0.3.js";
	jquery_el.onload = initialize;
	document.head.appendChild(jquery_el);
	
	function initialize () {
		$.ajax("/mode/4/1", {
			type: "POST"
		}).done(function (data) {
			var toggler = function () {
				$.ajax("/pin/4/1", {type:"POST"});
				setTimeout(function () {
					$.ajax("/pin/4/0", {type:"POST"});
					setTimeout(toggler, 50);
				}, 50);
			};
			toggler();
		});
	}
})();