
function map(value, fromMin, fromMax, toMin, toMax) {
  var norm;
  norm = void 0;
  value = parseFloat(value);
  fromMin = parseFloat(fromMin);
  fromMax = parseFloat(fromMax);
  toMin = parseFloat(toMin);
  toMax = parseFloat(toMax);
  norm = (value - fromMin) / (fromMax - fromMin).toFixed(2);
  return norm * (toMax - toMin) + toMin;
};


function drawTime(){
  var canvas = document.getElementById("clockblock");
  var ctx = canvas.getContext("2d");
  var width = canvas.width;
  var height = canvas.height;
  ctx.clearRect(0, 0, width, height)
  //ctx.arc(width/2, 45,10,0,2*Math.PI);
  //ctx.fill();

  var time = new Date();
  var hour = time.getHours();
  if ( hour > 12 )
    hour -= 12;
  $("#hour").text(hour);
  var minute = time.getMinutes();
  $("#minute").text(minute);
  minute /= 5;
  var second = time.getSeconds();
  $("#second").text(second);
  second /= 5;

  var nbr_circles = 12;
  
  var maxOpacity = 0.5;
  var defaultFill = "rgba(230,20,20,"+maxOpacity+")";
  // hours
  ctx.fillStyle = defaultFill;
  var lg_rad = (width/2) * .85;
  var lg_circ = 2*Math.PI*lg_rad;
  var sm_rad = 10;//(lg_circ / nbr_circles) / 2;
  var cx = width/2;
  var cy = height/2;
  for (var i = 0; i < nbr_circles; ++i) {
    if( i < hour ) {
      ctx.beginPath();
      var angle = (i - nbr_circles - 3 ) *2*Math.PI/nbr_circles;
      var x = cx + Math.cos(angle) * lg_rad;
      var y = cy + Math.sin(angle) * lg_rad;
      ctx.arc(x, y, sm_rad, 0, 2*Math.PI, false);
      ctx.fill();
    }
  }

  // minutes
  ctx.fillStyle = defaultFill;
  lg_rad = (width/2.5) * .85;
  lg_circ = 2*Math.PI*lg_rad;
  sm_rad = 10;//(lg_circ / nbr_circles) / 2;
  cx = width/2;
  cy = height/2;
  for (var i = 0; i < nbr_circles; ++i) {
    if( i <= Math.floor(minute) ) {
      if( i == Math.floor(minute) ) {
        var percent = minute - i;
        percent = map(percent, 0, 1, 0, maxOpacity);
        ctx.fillStyle = "rgba(255,0,0,"+percent+")";
      }
      ctx.beginPath();
      var angle = (i - nbr_circles - 3 ) *2*Math.PI/nbr_circles;
      var x = cx + Math.cos(angle) * lg_rad;
      var y = cy + Math.sin(angle) * lg_rad;
      ctx.arc(x, y, sm_rad, 0, 2*Math.PI, false);
      ctx.fill();
    }
  }

  // seconds
  ctx.fillStyle = defaultFill;
  lg_rad = (width/3.5) * .85;
  lg_circ = 2*Math.PI*lg_rad;
  sm_rad = 10;//(lg_circ / nbr_circles) / 2;
  cx = width/2;
  cy = height/2;
  for (var i = 0; i < nbr_circles; ++i) {
    if( i <= Math.floor(second) ) {
      if( i == Math.floor(second) ) {
        var percent = second - i;
        percent = map(percent, 0, 1, 0, maxOpacity);
        ctx.fillStyle = "rgba(255,0,0,"+percent+")";
      }
      ctx.beginPath();
      var angle = (i - nbr_circles - 3 ) *2*Math.PI/nbr_circles;
      var x = cx + Math.cos(angle) * lg_rad;
      var y = cy + Math.sin(angle) * lg_rad;
      ctx.arc(x, y, sm_rad, 0, 2*Math.PI, false);
      ctx.fill();
    }
  }
  window.setTimeout(function(){
    drawTime();
  }, 1000);
}

$(window).ready(function(){
  $("#clockblock").click(function(){
    var div = $(this).siblings(".time");
    if( div.css('display').match('block') )
      div.slideUp(400);
    else
      div.slideDown(400);
  });
  drawTime();
});
