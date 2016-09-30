/**
 * Created by ssm_jay on 2016-02-14.
 */
var express = require('express');
var router = express.Router();
var getIP = require('ipware')().get_ip;
var ImageDAO = require('../model/ImageDAO');
var SocketUtil = require('../util/SocketUtil');
var redisClient = require("../util/Redis");

var android_Ip = "211.189.20.198";
var another_Ip = "211.189.20.198";

router.post('/', function(req, res, next) {

    var req_data = JSON.stringify(req.body);
    var req_Obj = JSON.parse(req_data);
    var checkTarget = req_Obj.action_target;

    console.log(checkTarget);

    if(checkTarget=="requestOSDisplay")
        requestOSDisplay(res);
    else if(checkTarget=="requestOSList")
        requestOSList(req,res);
    else if(checkTarget=="requestCreateOS")
        requestCreateOS(req_Obj,req,res);
    else if(checkTarget=="requestDeleteOS")
        requestDeleteOS(req_Obj,req,res);
    else if(checkTarget=="requestStartOS")
        requestStartOS(req_Obj,req,res);
    else if(checkTarget=="requestInstallOS")
        requestInstallOS(req_Obj,req,res);

});


function requestOSList(req,res)
{
    ImageDAO.checkImageList(req.session.MemberEmail, function(result)
    {
        res.send(result);
    });
}

function requestDeleteOS(req_Obj,req,res) {

    var Index = req_Obj.Index;

    var Email = req.session.MemberEmail;

    var ImageVersion = req_Obj.ImageVersion;

    ImageDAO.deleteImage(Index, Email, ImageVersion, function(data) {

        if (ImageVersion == "androidx86")
        {
            SocketUtil.getSocketDelete(android_Ip, 2, getImageIndex(ImageVersion), Index, Email);
        }
        else {
            SocketUtil.getSocketDelete(another_Ip, 2, getImageIndex(ImageVersion), Index, Email);
        }
        res.sendStatus(200);
    });
}

function requestOSDisplay(res)
{
    res.render('select');
}

function requestCreateOS(req_Obj, req, res)
{
    var inputIndex;

    var Id = req.session.MemberEmail;

    var ImageVersion = req_Obj.ImageVersion;

    ImageDAO.initIndexers();

    ImageDAO.getIndexers("image_index",function(data)
    {
        inputIndex = data;

        ImageDAO.insertImage(inputIndex, req.session.MemberEmail, ImageVersion);

        if(ImageVersion=="androidx86")
        {
            SocketUtil.getSocketCreate(android_Ip, 1, getImageIndex(ImageVersion), inputIndex, Id);
        }
        else {

            SocketUtil.getSocketCreate(another_Ip, 1,
                getImageIndex(ImageVersion), inputIndex, Id);
        }

        res.sendStatus(200);
    });
}


function requestStartOS(req_Obj,req,res)
{
    var Index = req_Obj.Index;
    var ImageVersion = getImageIndex(req_Obj.ImageVersion);
    var Id = req.session.MemberEmail;
    var Ip = (getIP(req).clientIp);

    if(ImageVersion=="androidx86")
    {
        console.log("androidx86 execute Request");

        SocketUtil.getSocketStart(android_Ip, ImageVersion, Index, Id, Ip, 1, req.session.port, req.session.port, req.session.port);
    }
    else
    {

        console.log("another image execute Request");

        var check = true;

        checkOverLogin(Id,'sessionList1',function(result1) {


            if (result1 == true) {
                check = false;
            }

            else {
                checkOverLogin(Id, 'sessionList2', function (result2) {

                    if (result2 == true) {
                        check = false;
                    }
                });
            }

            if(check == false)
            {
                res.send("dup");
            }

            else{

                LoadBalancer(req,"sessionList1",function(result)
                {
                    if(result==true)
                    {
                    //    res.send("http://localhost:4000");
                        console.log(Ip);

                        SocketUtil.getSocketStart(another_Ip, 0, ImageVersion, Index, Id, Ip, 1, req.session.port, req.session.port, req.session.port);

                        res.send("http://211.189.20.76:3000"+" "+req.session.port);

                    }
                    else
                    {
                        res.send("full");
                    }
                });
            }
        });
    }
}


function requestInstallOS(req_Obj,req,res)
{
    var Index = req_Obj.Index;
    var ImageVersion = getImageIndex(req_Obj.ImageVersion);
    var Id = req.session.MemberEmail;
    var Ip = (getIP(req).clientIp);


    if(ImageVersion=="androidx86")
    {
        console.log("androidx86 execute Request");

    }
    else
    {
        console.log("another image execute Request");

        var check = true;

        checkOverLogin(Id,'sessionList1',function(result1) {


            if (result1 == true) {
                check = false;
            }

            else {
                checkOverLogin(Id, 'sessionList2', function (result2) {

                    if (result2 == true) {
                        check = false;
                    }
                });
            }

            if(check == false)
            {
                res.send("dup");
            }

            else{

                LoadBalancer(req,"sessionList2",function(result)
                {
                    if(result==true)
                    {
                        SocketUtil.getSocketInstall(another_Ip, 3, ImageVersion, Index, Id, Ip, 0, req.session.port, req.session.port, req.session.port);
                        //    res.send("http://localhost:4000");
                        res.send("http://211.189.20.76:3000"+" "+req.session.port);
                    }
                    else
                    {
                        res.send("full");
                    }
                });
            }
        });
    }
}


function getImageIndex(ImageVersion)
{
    var result;

    switch(ImageVersion)
    {
        case "androidx86" :
            result = 0;
            break;

        case "tizen" :
            result = 1;
            break;

        case "linux" :
            result = 2;
            break;

        case "window7" :
            result = 3;
            break;
    }

    return result;
}


function LoadBalancer(req,sessionList,callback)
{
    redisClient.getSessionList(sessionList, function(result)
    {
        var index = 1;

        for(index=1; index<=3; index++)
        {
            var target_index = 'port'+index;

            console.log("port"+index+" = "+result[target_index]);

            if(result[target_index]=="null")
            {
                break;
            }
        }

        if(index <= 3) {

            redisClient.setSession(sessionList,'port' + index, req.session.MemberEmail, function (result) {

                console.log(result);

                req.session.port = index;

                console.log("port = " +req.session.port);

                callback(true);
            });
        }
        else
        {
            callback(false);
        }
    });
}


function checkOverLogin(inputMemberEmail,sessionList,callback)
{
    redisClient.getSessionList(sessionList, function(result) {

        var index;

        for (index = 1; index <= 3; index++) {
            var target_index = 'port' + index;

            console.log("server1 port" + index + " = " + result[target_index]);

            if (result[target_index] == inputMemberEmail)
                break;
        }

        if(index > 3)
        {
            callback(false);
        }

        else {
            callback(true);
        }
    });
}

module.exports = router;
