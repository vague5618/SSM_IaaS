/**
 * Created by ssm_jay on 2016-02-04.
 */
/**
 * Created by ssm-jay on 16. 1. 2.
 */

var check = 0;


function event_init(ip,port_data)
{
    Event_client = new WebSocket(ip+(700+port_data));


    var canvas = document.getElementById("videoCanvas");

    canvas.addEventListener("touchstart", touchEvent_start, false);
    canvas.addEventListener("touchmove", touchEvent_move, false);
    canvas.addEventListener("touchend", touchEvent_end, false);
}


function scale_x(input_x)
{
    return input_x / window.innerWidth * 1280;
}

function scale_y(input_y)
{
    return input_y / window.innerHeight * 768;
}

function touchEvent_start(event)
{
    check = 1;

    if (Event_client != null) {

        var touchobj = event.changedTouches[0];

        var target_x = parseInt(scale_x(touchobj.clientX));

        var target_y = parseInt(scale_y(touchobj.clientY));

        console.log("down_ " + target_x + " " + target_y);

        Event_client.send("0" + " " + target_x + " " + target_y + " " + "1" + " ");
    }
}

function touchEvent_move(event)
{
    if(check==1) {

        if(Event_client!=null)
        {
            var touchobj = event.changedTouches[0];

            var target_x = parseInt(scale_x(touchobj.clientX));

            var target_y = parseInt(scale_y(touchobj.clientY));

            console.log("move_ " + target_x + " " + target_y);

            Event_client.send("0"+" "+target_x+" "+target_y+" "+"1"+" ");
        }
    }
}

function touchEvent_end(event)
{
        check = 0;

     if(Event_client!=null)
     {
         var touchobj = event.changedTouches[0];

         var target_x = parseInt(scale_x(touchobj.clientX));

         var target_y = parseInt(scale_y(touchobj.clientY));

         console.log("up_ " + target_x + " " + target_y);

         Event_client.send("0"+" "+target_x+" "+target_y+" "+"0"+" ");

    }
}

