function Decoder(bytes, port) {
    var birdGroups = [
        [
            "Luscinia megarhynchos",
            "Phylloscopus collybita",
            "Sylvia borin",
            "Strix aluco",
            "Acrocephalus palustris",
            "Sylvia atricapilla",
            "Rallus aquaticus",
            "Hippolais icterina",
            "Cettia cetti",
            "Phylloscopus trochilus",
            "Acrocephalus scirpaceus"
        ],
        [
            "Ekster",
            "Houtduif",
            "Huismus",
            "Kauw",
            "Koolmees",
            "Merel",
            "Pimpelmees",
            "Roodborst",
            "Turkse tortelduif",
            "Vink"
        ],
        [
            "Grutto (Limosa limosa)",
            "Kievit (Vanellus vanellus)",
            "Paapje (Saxicola rubetra)",
            "Tureluur (Tringa totanus)",
            "Scholekster (Haematopus ostralegus)",
            "Wulp (Numenius arquata)",
            "Watersnip (Gallinago gallinago)",
            "Kemphaan (Calidris pugnax)",
            "Slobeend (Spatula clypeata)",
            "Kwartelkoning (Crex crex)",
            "Kuifeend (Aythya fuligula)",
            "Zomertaling (Spatula querquedula)",
            "Wintertaling (Anas crecca)",
            "Veldleeuwerik (Alauda arvensis)",
            "Graspieper (Anthus pratensis)",
            "Gele kwikstaart (Motacilla flava)",
            "Kluut (Recurvirostra avosetta)",
            "Krakeend (Mareca strepera)",
            "Visdief (Sterna hirundo)",
            "Zwarte stern (Chlidonias niger)"
        ]
    ];

    //var bytes = input.bytes;
    var messageCount = bytes[0];
    var messageSize = bytes[1];
    var records = [];

    for (var i = 2; i < bytes.length ; i+= messageSize) {

        // Decode each field
        var birdList = bytes[i];
        var birdType = bytes[i + 1];
        var birdAccuracy = (bytes[i + 2] / 255 /* constants.uint8MaxValue */) * 100;
        var temperature = bytes[i + 3];
        var humidity = bytes[i + 4];
        var rainLastHour = bytes[i + 5];
        var batteryPercentage = bytes[i + 6];
        var validation = bytes[i + 7];
        var latitude = (((bytes[i + 8])) | ((bytes[i + 9]) << 8) | ((bytes[i+ 10]) << 16) | (bytes[i + 11] << 24)) / 10000000;
        var longitude = (((bytes[i + 12])) | ((bytes[i + 13]) << 8) | ((bytes[i+ 14]) << 16) | (bytes[i + 15] << 24)) / 10000000;
        var date = ((bytes[i + 16])) | ((bytes[i + 17]) << 8) | ((bytes[i + 18]) << 16) | (bytes[i + 19] << 24);
        var time = ((bytes[i + 20])) | ((bytes[i + 21]) << 8) | ((bytes[i + 22]) << 16) | (bytes[i + 23] << 24);
        var light = ((bytes[i + 24]) << 8) | ((bytes[i + 25]));

        function padStart(str, targetLength, padString) {
            while (str.length < targetLength) {
                str = padString + str;
            }
            return str;
        }
    
        // Construct the dateTime string
        var dateString = padStart(date.toString(), 8, '0');
        var timeString = padStart(time.toString(), 6, '0');
        var dateTime = dateString.substring(0, 4) + '-' + dateString.substring(4, 6) + '-' + dateString.substring(6, 8) + '_' + timeString.substring(0, 2) + '.' + timeString.substring(2, 4) + '.' + timeString.substring(4, 6);
        var date = Date.now();
        
        var bird = "test";
        if(birdList < 0 || birdList > 2 || birdType < 0 || birdType == null || birdList == null) {
          bird = "invalid";
        } else {
          var invalid = false;
          if(birdList == 0 && birdType > 12){
            invalid = true;
          }
          if(birdList == 1 && birdType > 10){
            invalid = true;
          }
          if(birdList == 2 && birdType > 20){
            invalid = true;
          }
          if(invalid){
            bird = "invalid";
          } else {
            bird = birdGroups[birdList][birdType];
          }
        }

        if(birdAccuracy == null){
            birdAccuracy = 0;
        }

        records.push(
            {   
                "field": "BIRD", 
                "value": bird,
                "timestamp": dateTime    
            });
        records.push(
            { 
                "field": "TEMPERATURE", 
                "value": temperature,
                "timestamp": dateTime    
            });
        records.push(
            { 
                "field": "HUMIDITY", 
                "value": humidity,
                "timestamp": dateTime    
            });  
        records.push(
            { 
                "field": "RAIN", 
                "value": rainLastHour,
                "timestamp": dateTime    
            });
        records.push(
            { 
                "field": "LIGHT", 
                "value": light,
                "timestamp": dateTime    
            });
        records.push(
            { 
                "field": "BATTERY", 
                "value": batteryPercentage,
                "timestamp": dateTime    
            });
        records.push(
            { 
                "field": "LATITUDE", 
                "value": latitude,
                "timestamp": dateTime    
            });
        records.push(
            { 
                "field": "LONGITUDE", 
                "value": longitude,
                "timestamp": dateTime    
            });
    }

    return records;
}
