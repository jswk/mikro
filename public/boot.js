(function () {
	var jquery_el = document.createElement("script");
	jquery_el.type = "text/javascript";
	jquery_el.src = "http://code.jquery.com/jquery-2.0.3.js";
	jquery_el.onload = initialize;
	document.head.appendChild(jquery_el);

	function initialize () {
		$("body").append("State: <div id='state'></div><br><div id='son'>Set on</div><br><div id='sof'>Set off</div>");
		$( "#son" ).click(function() {
				$.ajax("/pin/4/1", {type:"POST"});
		});
		$( "#sof" ).click(function() {
				$.ajax("/pin/4/0", {type:"POST"});
		});
		$.ajax("/mode/4/1", {
			type: "POST"
		}).done(function (data) {
			setInterval(function (){$.ajax("/pin/4", {type:"GET"}).done(
										function (data) {$("#state").text(data);})}
									,50);
				

		});
	}
})();
