/**
 * Created by ssm-jay on 16. 2. 22.
 */

function init()
{
    var action = "/select";

    var form_data = {
        action_target : "requestPort"
    };

    $.ajax({
        type: 'POST',
        data: JSON.stringify(form_data),
        contentType: 'application/json',
        url: action,
        success: function(data) {

            if(data!=null)
            {
                console.log("receive port = " + data);


  //              var target_ip = 'ws://211.189.19.30:';
 //                 var target_ip = 'ws://10.131.152.46:';
//                var target_ip = 'ws://192.168.0.3:';
      //          var target_ip = 'ws://172.20.10.13:';
                var target_ip = 'ws://211.189.20.76:';

                var port = data;

                image_init(target_ip,port);
                audio_init(target_ip,port);
                event_init(target_ip,port);
            }
            else
            {

            }
        }
    });
}

