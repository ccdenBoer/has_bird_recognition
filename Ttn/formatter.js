function decodeUplink(input) {
    var birdTypes = {
        1: "Wilde Eend",
        2: "Tjiftjaf",
        3: "Koolmees",
        4: "Houtduif",
        5: "Huismus",
        6: "Merel",
        7: "Winterkoning",
        8: "Fitis",
        9: "Vink",
        10: "Spreeuw",
        11: "Geen Vogel"
    };
    var data = input.bytes;
    var payload = {
        messageCounter: data[0],
        messageSize: data[1],
        messages: []
    };
    for (var i = 2; i < data.length; i += payload.messageSize) {
        payload.messages.push({
            birdType: birdTypes[data[i]],
            birdAccuracy: (data[i + 1] / 255 /* constants.uint8MaxValue */) * 100,
            lightIntensity: data[i + 2] << 8 | data[i + 3],
            temperature: data[i + 4],
            humidity: data[i + 5],
            raincoverage: data[i + 6],
            raining: data[i + 7],
            batteryPercentage: data[i + 8],
            lattitude: data[i + 9] << 8 | data[i + 10],
            longtitude: data[i + 11] << 8 | data[i + 12],
            validation: data[i + 10]
        });
    }
    return {
        data: payload,
        // Todo: add warnings from validation
        warnings: []
    };
}
