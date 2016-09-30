/**
 * Created by ubuntu1404-jay on 15. 12. 20.
 */


function onLoad() {

    $('#SelectModal').modal({
        backdrop: 'static',
        keyboard: false  // to prevent closing with Esc button (if you want this too)
    })

    requestOSList(0);
}

function goBack()
{
    history.go(-1);
}

function onSelectRadioChange()
{
    //alert($('input[name="selected_os"]:checked').parent().text());
}

function onCreateRadioChange()
{
    //alert($('input[name="created_os"]:checked').parent().text());
}

function clickAddImage()
{
    var check = $("#btn_plus").text();

    if(check == "+")
    {
        $("#selectList").fadeOut(function() {
            $("#createList").fadeIn();
            $("#btn_minus").fadeOut();
            $("#btn_install").fadeOut();

            $("#btn_plus").text("Back");
            $("#btn_select").text("Create");
            $('#btn_select').val('create');
        });
    }

    if(check == "Back")
    {
        $("#createList").fadeOut(function() {
            $("#btn_minus").fadeIn();
            $("#btn_install").fadeIn();

            $("#selectList").fadeIn();
            $("#btn_plus").text("+");
            $("#btn_select").text("Start");
            $('#btn_select').val('start');
        });
    }
}

function clickFinalImage()
{
    var type = $('#btn_select').attr('value');

    if(type=="start")
    {
        var selected_os = $('input[name="selected_os"]:checked').parent().text();

        var selected_index = $('input[name="selected_os"]:checked').attr('value');

        if(selected_os!=""){
            requestStartOS(selected_index, selected_os);
        }
        else {
            alert("OS를 선택 후 버튼을 눌러주세요.");
        }
    }
    else if(type=='create')
    {
        var selected_os = $('input[name="created_os"]:checked').parent().text().trim();

        if(selected_os!="") {
            //alert($('input[name="created_os"]:checked').parent().text());
        }

        requestCreateOS(selected_os);
    }
    else if(type=='delete')
    {
        var selected_os = $('input[name="selected_os"]:checked').parent().text();
        var selected_index = $('input[name="selected_os"]:checked').attr('value');

        if(selected_os!="") {
            //alert($('input[name="selected_os"]:checked').attr('value'));
        }

        requestDeleteOS(selected_os, selected_index);
    }

    else if(type=='install')
    {
        var selected_os = $('input[name="selected_os"]:checked').parent().text();

        var selected_index = $('input[name="selected_os"]:checked').attr('value');

        if(selected_os!=""){
            requestInstallOS(selected_index, selected_os);
        }
        else {
            alert("OS를 선택 후 버튼을 눌러주세요.");
        }
    }
}

function clickMinusImage()
{

    var check = $("#btn_minus").text();


    if(check == "-")
    {
     //   $("#selectList").fadeOut(function() {

            requestOSList(1);
            $("#btn_plus").fadeOut();
           $("#btn_install").fadeOut();

        //       $("#createList").fadeIn();
            $("#btn_minus").text("Back");
            $("#btn_select").text("Delete");
            $('#btn_select').val('delete');
     //   });
    }

    if(check == "Back")
    {
        requestOSList(0);
     //   $("#createList").fadeOut(function() {
            $("#btn_plus").fadeIn();
            $("#btn_install").fadeIn();
        //        $("#selectList").fadeIn();
            $("#btn_minus").text("-");
            $("#btn_select").text("Start");
            $('#btn_select').val('start');
     //   });
    }
}



function clickMinusImage()
{

    var check = $("#btn_minus").text();


    if(check == "-")
    {
        //   $("#selectList").fadeOut(function() {

        requestOSList(1);
        $("#btn_plus").fadeOut();
        $("#btn_install").fadeOut();

        //       $("#createList").fadeIn();
        $("#btn_minus").text("Back");
        $("#btn_select").text("Delete");
        $('#btn_select').val('delete');
        //   });
    }

    if(check == "Back")
    {
        requestOSList(0);
        //   $("#createList").fadeOut(function() {
        $("#btn_plus").fadeIn();
        $("#btn_install").fadeIn();

        //        $("#selectList").fadeIn();
        $("#btn_minus").text("-");
        $("#btn_select").text("Start");
        $('#btn_select').val('start');
        //   });
    }
}


function clickInstallImage()
{
    var check = $("#btn_install").text();

    if(check == "Install")
    {
        //   $("#selectList").fadeOut(function() {
        requestOSList(2);
        $("#btn_plus").fadeOut();
        $("#btn_minus").fadeOut();

        //       $("#createList").fadeIn();
        $("#btn_install").text("Back");
        $("#btn_select").text("Install");
        $('#btn_select').val('install');
        //   });
    }

    if(check == "Back")
    {
        requestOSList(0);
        //   $("#createList").fadeOut(function() {
        $("#btn_plus").fadeIn();
        $("#btn_minus").fadeIn();

        //        $("#selectList").fadeIn();
        $("#btn_install").text("Install");
        $("#btn_select").text("Start");
        $('#btn_select').val('start');
        //   });
    }
}


function requestOSList(type)
{
    var action = "/select";
    var form_data = {
        action_target : "requestOSList"
    };

    $.ajax({
        type: 'POST',
        data: JSON.stringify(form_data),
        contentType: 'application/json',
        url: action,
        success: function(data) {

            if(data!=null)
            {
                $('#selectList').empty();

                for(var index=0; index<data.length; index++)
                {
                    var req_data = JSON.stringify(data[index]);

                    //  var JSONObj = $.parseJSON(req_data);
                    var JSONObj = JSON.parse(req_data);

                    console.log(JSONObj.Email);
                    console.log(JSONObj.Image);
                    console.log(JSONObj.Index);

                    if(type==0) {
                        $('#selectList').append(
                            "<label class=\"element-animation1 btn btn-lg btn-success btn-block\"><span class=\"btn-label\"><i class=\"glyphicon glyphicon-chevron-right\"></i></span>" +
                            "<input type=\"radio\" name=\"selected_os\" value=" + JSONObj.Index + ">" + JSONObj.Image + "</label>"
                        )
                    }else if(type==1){
                        $('#selectList').append(
                            "<label class=\"element-animation1 btn btn-lg btn-danger btn-block\"><span class=\"btn-label\"><i class=\"glyphicon glyphicon-chevron-right\"></i></span>" +
                            "<input type=\"radio\" name=\"selected_os\" value=" + JSONObj.Index + ">" + JSONObj.Image + "</label>"
                        )
                    }else if(type==2) {
                        if (JSONObj.Image != "androidx86") {
                            $('#selectList').append(
                                "<label class=\"element-animation1 btn btn-lg btn-warning btn-block\"><span class=\"btn-label\"><i class=\"glyphicon glyphicon-chevron-right\"></i></span>" +
                                "<input type=\"radio\" name=\"selected_os\" value=" + JSONObj.Index + ">" + JSONObj.Image + "</label>"
                            );
                        }
                    }
                }
            }
            else
            {



            }
        }
    });
}

function requestCreateOS(targetVersion)
{
    var action = "/select";
    var form_data = {
        action_target : "requestCreateOS",
        ImageVersion : targetVersion
    };

    $.ajax({
        type: 'POST',
        data: JSON.stringify(form_data),
        contentType: 'application/json',
        url: action,
        success: function(data) {

            if(data!=null)
            {
                requestOSList(0);

                $("#createList").fadeOut(function() {
                    $("#btn_minus").fadeIn();
                    $("#btn_install").fadeIn();

                    $("#selectList").fadeIn();
                    $("#btn_plus").text("+");
                    $("#btn_select").text("Start");
                    $('#btn_select').val('start');
                });
            }
            else
            {
            }
        }
    });
}

function requestDeleteOS(selected_os, selected_index)
{
    var action = "/select";
    var form_data = {
        action_target : "requestDeleteOS",
        Index : selected_index,
        ImageVersion : selected_os
    };

    $.ajax({
        type: 'POST',
        data: JSON.stringify(form_data),
        contentType: 'application/json',
        url: action,
        success: function(data) {

            if(data!=null)
            {
                requestOSList(0);

                    $("#btn_plus").fadeIn();
                    $("#btn_install").fadeIn();
                    $("#btn_minus").text("-");
                    $("#btn_select").text("Start");
                    $('#btn_select').val('start');
            }
            else
            {

            }
        }
    });
}


function requestStartOS(index, imageVersion)
{
    var action = "/select";
    var form_data = {
        action_target : "requestStartOS",
        Index : index,
        ImageVersion : imageVersion
    };

    $.ajax({
        type: 'POST',
        data: JSON.stringify(form_data),
        contentType: 'application/json',
        url: action,
        success: function(data) {

            if(data=="dup")
            {
                alert("현재 접속 중인 아이디입니다.");
            }
            else if(data=="full")
            {
                alert("죄송합니다. 현재 가능한 서비스 인원이 찼습니다.");
            }
            else if(data!=null)
            {
                var res = data.split(" ");
                $.redirect(res[0],{'port':res[1]});
            }
        }
    });
}

function requestInstallOS(index, imageVersion)
{
    var action = "/select";
    var form_data = {
        action_target : "requestInstallOS",
        Index : index,
        ImageVersion : imageVersion
    };

    $.ajax({
        type: 'POST',
        data: JSON.stringify(form_data),
        contentType: 'application/json',
        url: action,
        success: function(data) {

            if(data=="dup")
            {
                alert("현재 접속 중인 아이디입니다.");
            }
            else if(data=="full")
            {
                alert("죄송합니다. 현재 가능한 서비스 인원이 찼습니다.");
            }
            else if(data!=null)
            {
                var res = data.split(" ");
                $.redirect(res[0],{'port':res[1]});
            }
        }
    });
}