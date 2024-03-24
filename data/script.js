setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var data = JSON.parse(this.responseText);
      console.log(data);
      document.querySelector('.humidity').textContent = `Độ ẩm: ${data.humidity} %`;
      document.querySelector('.temperature').textContent = `Nhiệt độ: ${data.temperature} °C`;
      document.querySelector('.led1').className = 't led1 ' + (data.led1 ? 'bat' : 'tat');
      document.querySelector('.led2').className = 't led2 ' + (data.led2 ? 'bat' : 'tat');
    }
  };
  xhttp.open('GET', '/data');
  xhttp.send();
}, 300);