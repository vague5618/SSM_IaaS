/**
 * Created by ubuntu1404-jay on 15. 12. 20.
 */

function loginMember()
{
    var action = "/member";
    var form_data = {
        action_target : "loginMember",
        inputMemberEmail : $("#inputMemberEmail").val(),
        inputMemberPassword : $("#inputMemberPassword").val()
    };

    $.ajax({
        type: 'POST',
        data: JSON.stringify(form_data),
        contentType: 'application/json',
        url: action,
        success: function(data) {

            if(data==false)
            {
                $("#loginAlert").css("display","");
            }
            else
            {
                toastr.success('Login Success');

                $.redirect('/select',{action_target : "requestOSDisplay"});

            }
        }
    });
}

function idCheckMember()
{
    if(registerInputCheck("inputRegisterEmail")==false)
    {
        //break;
    }

    else {
        var action = "/member";

        var form_data = {
            action_target: "idCheckMember",
            inputRegisterEmail: $("#inputRegisterEmail").val()
        };

        //ajax post method

        $.ajax({
            type: 'POST',
            data: JSON.stringify(form_data),
            contentType: 'application/json',
            url: action,
            success: function (data) {

                if (data == true) {
                    $('#possibleRegistration').html("이미 가입 된 아이디입니다.");
                    $('#possibleRegistration').css('color', 'red');
                    $('#possibleRegistration').effect('shake', 1000);
                }
                else {
                    $('#possibleRegistration').html("사용 가능한 아이디입니다.");
                    $('#possibleRegistration').css('color', 'green');
                }

            }
        });
    }
}

function joinMember()
{

    if(!(registerInputCheck("inputRegisterEmail") && registerInputCheck("inputRegisterPassword")
    && registerInputCheck("inputConfirmPassword") && registerInputCheck("inputRegisterFirstname")
        && registerInputCheck("inputRegisterLastname")))
    {
        //break;
    }

    else {

        if($("#inputRegisterPassword").val()!=($("#inputConfirmPassword").val()))
        {
            $('#possibleRegistration').html("입력 된 비밀번호가 다릅니다.");
            $('#possibleRegistration').css('color', 'red');
            $('#possibleRegistration').effect('shake', 1000 );
            return false;
        }

        var action = "/member";

        var form_data = {
            action_target: "joinMember",
            inputRegisterEmail: $("#inputRegisterEmail").val(),
            inputRegisterPassword: $("#inputRegisterPassword").val(),
            inputConfirmPassword: $("#inputConfirmPassword").val(),
            inputRegisterFirstname: $("#inputRegisterFirstname").val(),
            inputRegisterLastname: $("#inputRegisterLastname").val()
        };

        //ajax post method

        $.ajax({
            type: 'POST',
            data: JSON.stringify(form_data),
            contentType: 'application/json',
            url: action,
            success: function (data) {

                if (data == false) {


                }

                else {
                    toastr.success('Join Success');
                    window.setTimeout(function(){
                        window.location.href="/";
                    }, 800);
                }
            }
        });
    }
}

function registerInputCheck(target_id)
{
    var space_pattern = /\s/g;
    var number_pattern=/^[a-zA-Z]+$/;
    var kor_pattern = /[ㄱ-ㅎ|ㅏ-ㅣ|가-힣]/;
    //space check

    if (($("#"+target_id).val()).match(space_pattern)) {

        $('#possibleRegistration').html("입력 값에 공백이 들어갈 수 없습니다.");
        $('#possibleRegistration').css('color', 'red');
        $('#possibleRegistration').effect('shake', 1000 );

        return false;
    }

    //blank check
    else if(($("#"+target_id).val().replace(/\s|　/gi, ''))=='') {

        $('#possibleRegistration').html("입력 값에 공백이 들어갈 수 없습니다.");
        $('#possibleRegistration').css('color', 'red');
        $('#possibleRegistration').effect('shake', 1000 );
        return false;
    }

    else if((($("#"+target_id).val()).match(kor_pattern)))
    {
        $('#possibleRegistration').html("입력 값엔 영어와 숫자만 허용합니다.");
        $('#possibleRegistration').css('color', 'red');
        $('#possibleRegistration').effect('shake', 1000 );

        return false;
    }

    else if((($("#"+target_id).val()).length < 5))
    {
        $('#possibleRegistration').html("ID값은 5자 이상 입력이 필요합니다.");
        $('#possibleRegistration').css('color', 'red');
        $('#possibleRegistration').effect('shake', 1000 );

        return false;
    }

    else{

        return true;
    }
}

function registerPasswordCheck()
{
    if($("#inputRegisterPassword").val().length < 5)
    {
        $('#possibleRegistration').html("입력 된 비밀번호가 5자리 이상 입력이 필요합니다.");
        $('#possibleRegistration').css('color', 'red');
        $('#possibleRegistration').effect('shake', 1000 );
        return false;
    }

    else if($("#inputRegisterPassword").val()!=($("#inputConfirmPassword").val()))
    {
        $('#possibleRegistration').html("입력 된 비밀번호가 다릅니다.");
        $('#possibleRegistration').css('color', 'red');
        $('#possibleRegistration').effect('shake', 1000 );
        return false;
    }
    else{

        $('#possibleRegistration').html("가능한 비밀번호입니다.");
        $('#possibleRegistration').css('color', 'green');
        $('#possibleRegistration').effect('shake', 1000 );
        return true;
    }
}


function registerNameCheck()
{

    if($("#inputRegisterFirstname").val()=="" || $("#inputRegisterLastname").val()=="")
    {
        $('#possibleRegistration').html("이름엔 공백이 들어갈 수 없습니다.");
        $('#possibleRegistration').css('color', 'red');
        $('#possibleRegistration').effect('shake', 1000 );
        return false;
    }

    else{

        $('#possibleRegistration').html("가능한 이름입니다.");
        $('#possibleRegistration').css('color', 'green');
        $('#possibleRegistration').effect('shake', 1000 );
        return true;
    }
}