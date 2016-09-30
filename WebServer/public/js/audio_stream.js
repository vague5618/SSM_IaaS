/**
 * Created by LDCC on 2016-01-18.
 */


///////////Audio_Stream////////////////////////


var context = new AudioContext();

var AudioBuffer = context.createBuffer(2,4096,44100);

var  data_buffer = new ArrayBuffer();


function audio_init(ip,port_data)
{

    Audio_client = new BinaryClient(ip+(950+port_data));

    Audio_client.on('stream', function(stream, meta) {

        console.log("=========Audio_stream create=========");

        stream.on('data', function (data) {

            //    //var d1 = _base64ToArrayBuffer(data);
            //    //
            //    //var d2 = new DataView(d1);
            //    //
            //    //var data = new Float32Array(d2.byteLength / Float32Array.BYTES_PER_ELEMENT);
            //    //
            //    //for (var jj = 0; jj < data.length; ++jj) {
            //    //    data[jj] = d2.getFloat32(jj * Float32Array.BYTES_PER_ELEMENT, true);
            //    //}
            //    //
            //    //var audioBuffer = context.createBuffer(2, data.length, 44100);
            //    //audioBuffer.getChannelData(0).set(data);
            //    //
            //    //var source = context.createBufferSource(); // creates a sound source
            //    //source.buffer = audioBuffer;
            //    //source.connect(context.destination); // connect the source to the context's destination (the speakers)
            //    //source.start(0);
            //
            //});

            console.log("=====Audio_stream_received======");

            data_buffer = _appendBuffer(data_buffer, data);

            console.log(data_buffer.byteLength);

            if (data_buffer.byteLength > 418 * 20) {

                context.decodeAudioData(data_buffer, function (res) {

                    AudioBuffer = appendBuffer(AudioBuffer, res);

                    AudioSource = context.createBufferSource();

                    AudioSource.connect(context.destination);

                    AudioSource.buffer = AudioBuffer;

                    AudioSource.start(0);

                    AudioSource.playbackRate = 1;

                });

                data_buffer = new ArrayBuffer();

                AudioBuffer = context.createBuffer(2,4096,44100);
            }
        });

        stream.on('close', function(){

            console.log("=====Audio_stream_end======");

        });

    });
}

var _appendBuffer = function(buffer1, buffer2) {
    var tmp = new Uint8Array(buffer1.byteLength + buffer2.byteLength);
    tmp.set(new Uint8Array(buffer1), 0);
    tmp.set(new Uint8Array(buffer2), buffer1.byteLength);
    return tmp.buffer;
};


function appendBuffer(buffer1, buffer2) {
    var numberOfChannels = Math.min( buffer1.numberOfChannels, buffer2.numberOfChannels );
    var tmp = context.createBuffer( numberOfChannels, (buffer1.length + buffer2.length), buffer1.sampleRate );
    for (var i=0; i<numberOfChannels; i++) {
        var channel = tmp.getChannelData(i);
        channel.set( buffer1.getChannelData(i), 0);
        channel.set( buffer2.getChannelData(i), buffer1.length);
    }
    return tmp;
}