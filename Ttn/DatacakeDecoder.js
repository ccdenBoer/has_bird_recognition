function Decoder(bytes, port) {
  
  // Convert bytes to floating-point numbers
  var data = bytes;
  var temp = data[0] << 8 | data[1];
  var hum = data[2] << 8 | data[3];
  var windDir = data[4] << 8 | data[5];
  var averageWindSpeed = data[6] << 8 | data[7];
  var topWindSpeed = data[8] << 8 | data[9];
  var rainMinuteMM = data[10] << 8 | data[11];
  var pressure = data[12] << 8 | data[13];
  
  switch (windDir) {
      case 0 :
        var windDirText = "N";
        break;
      case 23 :
        var windDirText = "NNO";
        break;
      case 45 :
        var windDirText = "NO";
        break;
      case 68 :
        var windDirText = "NOO";
        break;
      case 90 :
        var windDirText = "O";
        break;
      case 113 :
        var windDirText = "ZOO";
        break;
      case 135 :
        var windDirText = "ZO";
        break;
      case 158 :
        var windDirText = "ZZO";
        break;
      case 180 :
        var windDirText = "Z";
        break;
      case 203 :
        var windDirText = "ZZW";
        break;
      case 225 :
        var windDirText = "ZW";
        break;
      case 248 :
        var windDirText = "ZWW";
        break;
      case 270 :
        var windDirText = "W";
        break;
      case 293 :
        var windDirText = "NWW";
        break;
      case 315 :
        var windDirText = "NW";
        break;
      case 338 :
        var windDirText = "NNW";
        break;
    default:
      var windDirText = "NaN";
  }

  payload = [
  { "field": "TEMPERATURE", "value": temp / 10 },
  { "field": "HUMIDITY", "value": hum / 10 },    
  { "field": "WINDDIRECTION", "value": windDir /10 },    
  { "field": "WINDDIRECTIONTEXT", "value": windDirText }, 
  { "field": "AVERAGEWINDSPEED", "value": averageWindSpeed /10 },    
  { "field": "TOPWINDSPEED", "value": topWindSpeed /10 },    
  { "field": "RAINMINUTEMM", "value": rainMinuteMM /10 },    
  { "field": "PRESSURE", "value": pressure /10 },    
  ];
   
  return payload;
}