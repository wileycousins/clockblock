
@a = @a || {}

@map = (value, fromMin, fromMax, toMin, toMax) ->
  norm = undefined
  value = parseInt(value)
  fromMin = parseInt(fromMin)
  fromMax = parseInt(fromMax)
  toMin = parseInt(toMin)
  toMax = parseInt(toMax)
  norm = (value - fromMin) / (fromMax - fromMin).toFixed(1)
  norm * (toMax - toMin) + toMin

Array.prototype.remove = (from, to) ->
  rest = @.slice((to || from) + 1 || @.length)
  if from < 0
    @.length = @.length + from
  else
    @.length = from
  return @.push.apply(@, rest)

@random = (min, max) ->
  if !max
    max = min
    min = 0
  Math.round Math.random() * (max - min) + min

@slugify = (value) ->
  value.toLowerCase().replace(/-+/g, "").replace(/\s+/g, "-").replace /[^a-z0-9-]/g, ""
