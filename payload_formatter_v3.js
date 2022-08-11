function decodeUplink(input) {
    var data = {};

    if (input.bytes[0] != 0xff) {
        data.humidity = input.bytes[2] | ((input.bytes[1] & 0x0f) << 8);
        data.humidity /= 10
        data.temperature = ((input.bytes[1] & 0xf0) >> 4) | (input.bytes[0] << 4)
        data.temperature -= 400;
        data.temperature /= 10;
    }

    data.batVoltage = input.bytes[3]
    data.batVoltage += 250
    data.batVoltage /= 100

    return { data };
}
