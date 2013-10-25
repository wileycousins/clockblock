
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
  // refresh rate
  var refreshRate = 30;
  var refreshTime = (1/refreshRate)*1000;

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
  else if ( hour === 0 )
    hour = 12
  $("#hour").text(hour);
  var minute = time.getMinutes();
  $("#minute").text(minute);
  var second = time.getSeconds();
  $("#second").text(second);
  var milli = time.getMilliseconds();

  var nbr_circles = 12;
  var minOpacity = 0.1;
  var maxOpacity = 0.5;
  var defaultFill = "rgba(230,20,20,"+maxOpacity+")";

  // percent fills mapped to opacity
  var hourPercent = (((milli+1)/1000) + second + (minute*60))/3600.0;
  var minPercent  = (((milli+1)/1000) + second + ((minute%5)*60))/(5*60.0);
  var secPercent  = (((milli+1)/1000) + (second%5))/5.0;
  hourPercent = map(hourPercent, 0, 1, minOpacity, maxOpacity);
  minPercent  = map(minPercent, 0, 1, minOpacity, maxOpacity);    
  secPercent  = map(secPercent, 0, 1, minOpacity, maxOpacity);

  // replicate the overflow animation from hardware
  var secStart = 0;
  var minStart = 0;
  var hourStart = 0;
  if ( (second === 59) && (milli >= (1000 - (nbr_circles*refreshTime))) ) {
    secStart = Math.floor((milli - (1000 - (nbr_circles*refreshTime)))/refreshTime);
    if (minute == 59) {
      minStart = secStart;
      if (hour === 11) {
        hourStart = secStart;
      }
    }
  }

  // convert times to hands
  hour = hour % 12;
  minute = Math.floor(minute / 5);
  second = Math.floor(second / 5);

  // hours
  ctx.fillStyle = defaultFill;
  var lg_rad = (width/2) * .85;
  var lg_circ = 2*Math.PI*lg_rad;
  var sm_rad = 10;//(lg_circ / nbr_circles) / 2;
  var cx = width/2;
  var cy = height/2;
  for (var i = hourStart; i <= hour; ++i) {
    if( i === hour )
      ctx.fillStyle = "rgba(230,20,20,"+hourPercent+")";
    ctx.beginPath();
    var angle = (i - nbr_circles - 3 ) *2*Math.PI/nbr_circles;
    var x = cx + Math.cos(angle) * lg_rad;
    var y = cy + Math.sin(angle) * lg_rad;
    ctx.arc(x, y, sm_rad, 0, 2*Math.PI, false);
    ctx.fill();
  }

  // minutes
  ctx.fillStyle = defaultFill;
  lg_rad = (width/2.5) * .85;
  lg_circ = 2*Math.PI*lg_rad;
  sm_rad = 10;//(lg_circ / nbr_circles) / 2;
  cx = width/2;
  cy = height/2;
  for (var i = minStart; i <= minute; ++i) {
    if( i === minute )
      ctx.fillStyle = "rgba(230,20,20,"+minPercent+")";
    ctx.beginPath();
    var angle = (i - nbr_circles - 3 ) *2*Math.PI/nbr_circles;
    var x = cx + Math.cos(angle) * lg_rad;
    var y = cy + Math.sin(angle) * lg_rad;
    ctx.arc(x, y, sm_rad, 0, 2*Math.PI, false);
    ctx.fill();
  }

  // seconds
  ctx.fillStyle = defaultFill;
  lg_rad = (width/3.5) * .85;
  lg_circ = 2*Math.PI*lg_rad;
  sm_rad = 10;//(lg_circ / nbr_circles) / 2;
  cx = width/2;
  cy = height/2;
  for (var i = secStart; i <= second; ++i) {
    if( i === second )
      ctx.fillStyle = "rgba(230,20,20,"+secPercent+")";
    ctx.beginPath();
    var angle = (i - nbr_circles - 3 ) *2*Math.PI/nbr_circles;
    var x = cx + Math.cos(angle) * lg_rad;
    var y = cy + Math.sin(angle) * lg_rad;
    ctx.arc(x, y, sm_rad, 0, 2*Math.PI, false);
    ctx.fill();
  }
  window.setTimeout(function(){
    drawTime();
  }, refreshTime);
}

function checkSize(){
  var w = 400;
  if( $(window).width() < 389 ){
    w = 256;
  }
  else if( $(window).width() < 460 ){
    w = 290;
  }
  else if( $(window).width() < 760 ){
    w = 337;
  }
  $("#clockblock").width(w);
  $("#clockblock-block img").width(w);
}

$(window).resize(checkSize);
$(window).ready(function(){
  checkSize();
  $("#clockblock-block").click(function(){
    var div = $(this).find(".time");
    if( !div.data('out') ){
      div.removeClass("short");
      //div.animate({
        //height: 120
      //},400);
      div.data('out', true);
    }
    else {
      div.addClass("short");
      //div.animate({
        //height: 0
      //},400);
      div.data('out', false);
    }
  });
  drawTime();
});
