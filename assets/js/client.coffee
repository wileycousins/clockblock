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
  spacing_multi = 0.90
  minOpacity = 0.2
  
  # blend mode needs to go to zero
  minOpacity = 0  if displayMode is 2
  maxOpacity = 0.6
  defaultFill = "rgba(230,20,20," + maxOpacity + ")"
  
  # percent fills
  hourPercent = 0
  minPercent = 0
  secPercent = 0
  if displayMode is 0 or displayMode is 1
    hourPercent = (((milli + 1) / 1000) + second + (minute * 60)) / 3600.0
    minPercent = (((milli + 1) / 1000) + second + ((minute % 5) * 60)) / (5 * 60.0)
    secPercent = (((milli + 1) / 1000) + (second % 5)) / 5.0
  else if displayMode is 2
    hourPercent = ((milli / 1000) + second + (minute * 60)) / 3600.0
    minPercent = ((milli / 1000) + second + ((minute % 5) * 60)) / (5 * 60.0)
    secPercent = ((milli / 1000) + (second % 5)) / 5.0
  
  # map the percents to opacity
  hourBrightness = map(hourPercent, 0, 1, minOpacity, maxOpacity)
  minBrightness = map(minPercent, 0, 1, minOpacity, maxOpacity)
  secBrightness = map(secPercent, 0, 1, minOpacity, maxOpacity)
  secStart = 0
  minStart = 0
  hourStart = 0
  
  # replicate the overflow animation from hardware for fill mode
  if displayMode is 0
    if (second is 59) and (milli >= (1000 - (nbr_circles * refreshTime)))
      secStart = Math.floor((milli - (1000 - (nbr_circles * refreshTime))) / refreshTime)
      if minute is 59
        minStart = secStart
        hourStart = secStart  if hour is 11
  
  # convert times to hands
  hour = hour % 12
  minute = Math.floor(minute / 5)
  second = Math.floor(second / 5)
  if displayMode is 1 or displayMode is 2
    hourStart = hour
    minStart = minute
    secStart = second
  if displayMode is 2
    hour += 1
    minute += 1
    second += 1
  
  # hours
  ctx.fillStyle = defaultFill
  lg_rad = (width / 4) * spacing_multi
  lg_circ = 2 * Math.PI * lg_rad
  sm_rad = 10 #(lg_circ / nbr_circles) / 2;
  cx = width / 2
  cy = height / 2
  i = hourStart

  while i <= hour
    brightness = map((1 - hourPercent), 0, 1, minOpacity, maxOpacity)  if displayMode is 2 and i is hourStart
    ctx.fillStyle = "rgba(230,20,20," + brightness + ")"
    ctx.fillStyle = "rgba(230,20,20," + hourBrightness + ")"  if i is hour
    ctx.beginPath()
    angle = (i - nbr_circles - 3) * 2 * Math.PI / nbr_circles
    x = cx + Math.cos(angle) * lg_rad
    y = cy + Math.sin(angle) * lg_rad
    ctx.arc x, y, sm_rad, 0, 2 * Math.PI, false
    ctx.fill()
    ++i
  
  # minutes
  ctx.fillStyle = defaultFill
  lg_rad = (width / 2.65) * spacing_multi
  lg_circ = 2 * Math.PI * lg_rad
  sm_rad = 10 #(lg_circ / nbr_circles) / 2;
  cx = width / 2
  cy = height / 2
  i = minStart

  while i <= minute
    brightness = map((1 - minPercent), 0, 1, minOpacity, maxOpacity)  if displayMode is 2 and i is minStart
    ctx.fillStyle = "rgba(230,20,20," + brightness + ")"
    ctx.fillStyle = "rgba(230,20,20," + minBrightness + ")"  if i is minute
    ctx.beginPath()
    angle = (i - nbr_circles - 3) * 2 * Math.PI / nbr_circles
    x = cx + Math.cos(angle) * lg_rad
    y = cy + Math.sin(angle) * lg_rad
    ctx.arc x, y, sm_rad, 0, 2 * Math.PI, false
    ctx.fill()
    ++i
  
  # seconds
  ctx.fillStyle = defaultFill
  lg_rad = (width / 2) * spacing_multi
  lg_circ = 2 * Math.PI * lg_rad
  sm_rad = 10 #(lg_circ / nbr_circles) / 2;
  cx = width / 2
  cy = height / 2
  i = secStart

  while i <= second
    brightness = map((1 - secPercent), 0, 1, minOpacity, maxOpacity)  if displayMode is 2 and i is secStart
    ctx.fillStyle = "rgba(230,20,20," + brightness + ")"
    ctx.fillStyle = "rgba(230,20,20," + secBrightness + ")"  if i is second
    ctx.beginPath()
    angle = (i - nbr_circles - 3) * 2 * Math.PI / nbr_circles
    x = cx + Math.cos(angle) * lg_rad
    y = cy + Math.sin(angle) * lg_rad
    ctx.arc x, y, sm_rad, 0, 2 * Math.PI, false
    ctx.fill()
    ++i
  window.setTimeout (->
    drawTime()
  ), refreshTime


checkSize = ->
  w = 400
  if $(window).width() < 389
    w = 256
  else if $(window).width() < 460
    w = 290
  else if $(window).width() < 760
    w = 337  
  $("#clockblock-block").width w
  $("#clockblock").width w
  $("#clockblock-block img").width w

displayMode = 0
numModes = 3

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
  $("[name='version']").change (e)->
    el = $(e.currentTarget)
    total = parseFloat(el.val())+ 12.35
    $(".total").text total
    $("[name='total']").val total
    $(".model-imgs img").addClass 'hidden'
    $($(".model-imgs img")[el[0].selectedIndex]).removeClass 'hidden'
    $(".model-descriptions > div").addClass 'hidden'
    $($(".model-descriptions > div")[el[0].selectedIndex]).removeClass 'hidden'

  $("#payment-form").submit (e) ->
    form = $(@)
    form.find('button').prop 'disabled', true
    Stripe.card.createToken form, stripeResHandler
    false

$(window).resize checkSize
$(window).ready ->
  bindPaymentForm()
  $(".model").trigger 'change'
  checkSize()
  drawTime()
  $(".h30").click (e) ->
    $('.radio.checked').removeClass 'checked'
    radio = $(@).find '.radio'
    displayMode = parseInt(radio.data 'val' )
    radio.addClass 'checked'

  $(".img-thumbnail").click (e)->
    el = $(e.currentTarget)
    $(".modal-body").css 'background-image', "url(#{el.attr('src')})"
    $("#myModal").modal 'toggle'
