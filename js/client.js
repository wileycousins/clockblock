function subscribeEmail(){
  $.post($('#email-signup').data('action'), 'EMAIL='+$('#email-signup').val(), function(res, data){
    console.log(res);
    console.log(data);
  });
}
$(window).ready(function(){
  $("#email-signup-button").click(function(){
    subscribeEmail();
  });
});
