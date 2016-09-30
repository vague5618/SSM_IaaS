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

    canvas.addEventListener("mousedown", mouseEvent_down, false);
    canvas.addEventListener("mousemove", mouseEvent_move, false);
    canvas.addEventListener("mouseup", mouseEvent_up, false);
    canvas.addEventListener("mouseout", mouseEvent_out, false);
    canvas.addEventListener("contextmenu",function(e)
    {
        e.preventDefault();
    },false);

    document.addEventListener("keydown",function(e)
    {
        if(event.KeyCode = 122)
            return true;

        e.preventDefault();

    },false);

}


function scale_x(input_x)
{
    return input_x / window.innerWidth * 1280;
}

function scale_y(input_y)
{
    return input_y / window.innerHeight * 768;
}

function keydown(event)
{
    var key = event.KeyCode || event.which;

    var keychar = String.fromCharCode(key);

    console.log("keyDown = "+ keychar+" "+key);

    if(Event_client!=null) {
        Event_client.send("1" + " " + "1" + " " + key);
    }
}

function keyup(event)
{
    var key = event.KeyCode || event.which;

    var keychar = String.fromCharCode(key);

    console.log("keyUP = "+ keychar+" "+key);

    if(Event_client!=null) {
        Event_client.send("1" + " " + "0" + " " + key);
    }
}

function mouseEvent_down(event)
{
        if(event.which == 1) {
            check = 1;

            if (Event_client != null) {
                var target_x = parseInt(scale_x(event.clientX));

                var target_y = parseInt(scale_y(event.clientY));

                console.log("down_ " + target_x + " " + target_y);

                Event_client.send("0" + " " + target_x + " " + target_y + " " + "1" + " ");
            }
        }

        if(event.which == 3) {

            event.preventDefault();

            check = 4;

        if (Event_client != null) {
            var target_x = parseInt(scale_x(event.clientX));

            var target_y = parseInt(scale_y(event.clientY));

            console.log("mouse " +
                "right down_ " + target_x + " " + target_y);

            Event_client.send("0" + " " + target_x + " " + target_y + " " + "4" + " ");
         }
        }
}

function mouseEvent_move(event)
{
    if(check==1) {

        if(Event_client!=null)
        {
            var target_x = parseInt(scale_x(event.clientX));

            var target_y =  parseInt(scale_y(event.clientY));

            console.log("move_ " + target_x + " " + target_y);

            Event_client.send("0"+" "+target_x+" "+target_y+" "+"1"+" ");
        }
    }
}

function mouseEvent_up(event)
{
        check = 0;

     if(Event_client!=null)
     {
         var target_x = parseInt(scale_x(event.clientX));

         var target_y =  parseInt(scale_y(event.clientY));

         console.log("up_ " + target_x + " " + target_y);

         Event_client.send("0"+" "+target_x+" "+target_y+" "+"0"+" ");

    }
}

function mouseEvent_out(event)
{
    if(Event_client!=null && check==1)
    {
        var target_x = parseInt(scale_x(event.clientX));

        var target_y =  parseInt(scale_y(event.clientY));

        console.log("out_ " + target_x + " " + target_y);

        Event_client.send("0"+" "+target_x+" "+target_y+" "+"0"+" ");
    }

    check = 0;
}
