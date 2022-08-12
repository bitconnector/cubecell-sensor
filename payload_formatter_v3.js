function getBat(input, offset) {
    var batVoltage = input.bytes[offset]
    batVoltage += 250
    batVoltage /= 100
    return { batVoltage };
}

function getPin(input, offset) {
    var button = input.bytes[offset]
    return { button }
}

function getTempHum(input, offset) {
    var humidity = input.bytes[offset + 2] | ((input.bytes[offset + 1] & 0x0f) << 8);
    humidity /= 10
    var temperature = ((input.bytes[offset + 1] & 0xf0) >> 4) | (input.bytes[offset] << 4)
    temperature -= 400;
    temperature /= 10;
    return { humidity, temperature }
}

function getPress(input, offset) {
    var pressure = (input.bytes[offset] << 8) | (input.bytes[offset + 1])
    pressure += 15000;
    pressure /= 50;
    return { pressure }
}


function decodeUplink(input) {
    var data = {};

    data = getBat(input, 0)

    if (input.fPort == 1) {
        data = Object.assign(data, getPin(input, 1))
    }

    else if (input.fPort == 2) {
        data = Object.assign(data, getTempHum(input, 1))
    }

    else if (input.fPort == 3) {
        data = Object.assign(data, getTempHum(input, 1))
        data = Object.assign(data, getPress(input, 4))
    }


    return { data };
}
