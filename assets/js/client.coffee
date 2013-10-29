map = (value, fromMin, fromMax, toMin, toMax) ->
  norm = undefined
  norm = undefined
  value = parseFloat(value)
  fromMin = parseFloat(fromMin)
  fromMax = parseFloat(fromMax)
  toMin = parseFloat(toMin)
  toMax = parseFloat(toMax)
  norm = (value - fromMin) / (fromMax - fromMin).toFixed(2)
  norm * (toMax - toMin) + toMin
drawTime = ->
  
  # refresh rate
  refreshRate = 30
  refreshTime = (1 / refreshRate) * 1000
  canvas = document.getElementById("clockblock")
  ctx = canvas.getContext("2d")
  width = canvas.width
  height = canvas.height
  ctx.clearRect 0, 0, width, height
  
  #ctx.arc(width/2, 45,10,0,2*Math.PI);
  #ctx.fill();
  time = new Date()
  hour = time.getHours()
  if hour > 12
    hour -= 12
  else hour = 12  if hour is 0
  $("#hour").text hour
  minute = time.getMinutes()
  $("#minute").text minute
  second = time.getSeconds()
  $("#second").text second
  milli = time.getMilliseconds()
  nbr_circles = 12
  minOpacity = 0.1
  maxOpacity = 0.5
  defaultFill = "rgba(230,20,20," + maxOpacity + ")"
  
  # percent fills mapped to opacity
  hourPercent = (((milli + 1) / 1000) + second + (minute * 60)) / 3600.0
  minPercent = (((milli + 1) / 1000) + second + ((minute % 5) * 60)) / (5 * 60.0)
  secPercent = (((milli + 1) / 1000) + (second % 5)) / 5.0
  hourPercent = map(hourPercent, 0, 1, minOpacity, maxOpacity)
  minPercent = map(minPercent, 0, 1, minOpacity, maxOpacity)
  secPercent = map(secPercent, 0, 1, minOpacity, maxOpacity)
  
  # replicate the overflow animation from hardware
  secStart = 0
  minStart = 0
  hourStart = 0
  if (second is 59) and (milli >= (1000 - (nbr_circles * refreshTime)))
    secStart = Math.floor((milli - (1000 - (nbr_circles * refreshTime))) / refreshTime)
    if minute is 59
      minStart = secStart
      hourStart = secStart  if hour is 11
  
  # convert times to hands
  hour = hour % 12
  minute = Math.floor(minute / 5)
  second = Math.floor(second / 5)
  
  # hours
  ctx.fillStyle = defaultFill
  lg_rad = (width / 2) * .85
  lg_circ = 2 * Math.PI * lg_rad
  sm_rad = 10 #(lg_circ / nbr_circles) / 2;
  cx = width / 2
  cy = height / 2
  i = hourStart

  while i <= hour
    ctx.fillStyle = "rgba(230,20,20," + hourPercent + ")"  if i is hour
    ctx.beginPath()
    angle = (i - nbr_circles - 3) * 2 * Math.PI / nbr_circles
    x = cx + Math.cos(angle) * lg_rad
    y = cy + Math.sin(angle) * lg_rad
    ctx.arc x, y, sm_rad, 0, 2 * Math.PI, false
    ctx.fill()
    ++i
  
  # minutes
  ctx.fillStyle = defaultFill
  lg_rad = (width / 2.5) * .85
  lg_circ = 2 * Math.PI * lg_rad
  sm_rad = 10 #(lg_circ / nbr_circles) / 2;
  cx = width / 2
  cy = height / 2
  i = minStart

  while i <= minute
    ctx.fillStyle = "rgba(230,20,20," + minPercent + ")"  if i is minute
    ctx.beginPath()
    angle = (i - nbr_circles - 3) * 2 * Math.PI / nbr_circles
    x = cx + Math.cos(angle) * lg_rad
    y = cy + Math.sin(angle) * lg_rad
    ctx.arc x, y, sm_rad, 0, 2 * Math.PI, false
    ctx.fill()
    ++i
  
  # seconds
  ctx.fillStyle = defaultFill
  lg_rad = (width / 3.5) * .85
  lg_circ = 2 * Math.PI * lg_rad
  sm_rad = 10 #(lg_circ / nbr_circles) / 2;
  cx = width / 2
  cy = height / 2
  i = secStart

  while i <= second
    ctx.fillStyle = "rgba(230,20,20," + secPercent + ")"  if i is second
    ctx.beginPath()
    angle = (i - nbr_circles - 3) * 2 * Math.PI / nbr_circles
    x = cx + Math.cos(angle) * lg_rad
    y = cy + Math.sin(angle) * lg_rad
    ctx.arc x, y, sm_rad, 0, 2 * Math.PI, false
    ctx.fill()
    ++i
  window.setTimeout ->
    drawTime()
  , refreshTime

checkSize = ->
  w = 400
  if $(window).width() < 389
    w = 256
  else if $(window).width() < 460
    w = 290
  else w = 337  if $(window).width() < 760
  $("#clockblock").width w
  $("#clockblock-block img").width w

stripeResHandler = (status, res) ->
  form = $("#payment-form")
  if res.error
    form.find('.payment-errors').text res.error.message
    form.find('button').prop 'disabled', false
    #form.find('button').removeAttr 'disabled'
  else
    form.append $("<input type='hidden' name='stripeToken' value='#{res.id}'/>")
    form.get(0).submit()

bindPaymentForm = ->
  $("#payment-form").submit (e) ->
    form = $(@)
    form.find('button').prop 'disabled', true
    Stripe.card.createToken form, stripeResHandler
    false

$(window).resize checkSize

$(window).ready ->
  bindPaymentForm()
  checkSize()
  $("#clockblock-block").click ->
    div = $(this).find(".time")
    unless div.data("out")
      div.removeClass "short"
      
      #div.animate({
      #height: 120
      #},400);
      div.data "out", true
    else
      div.addClass "short"
      
      #div.animate({
      #height: 0
      #},400);
      div.data "out", false

  drawTime()

