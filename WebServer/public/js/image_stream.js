/**
 * Created by LDCC on 2016-01-18.
 */
// Setup the WebSocket connection and start the player


/////////////Image_Stream////////////////////////


function image_init(ip,port_data) {

    var canvas = document.getElementById('videoCanvas');

    var Image_client = new WebSocket(ip + (900 + port_data));

    var player = new jsmpeg(Image_client, {canvas: canvas});
}


