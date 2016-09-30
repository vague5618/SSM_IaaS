var express = require('express');
var ip = require('ip');
var router = express.Router();
var MemberDAO = require('../model/MemberDAO');
var SocketUtil = require('../util/SocketUtil');
var redisClient = require('../util/Redis');
/* GET home page. */
router.post('/', function(req, res) {

   var req_data = JSON.stringify(req.body);
   var checkMember = JSON.parse(req_data);
   var checkTarget = checkMember.action_target;

   if(checkTarget=="loginMember")
      loginMember(checkMember,req,res);
   else if(checkTarget=="idCheckMember")
      idCheckMember(checkMember,res);
   else if(checkTarget=="joinMember")
      joinMember(checkMember,res);
   else if(checkTarget=="mobile_loginMember") {
      mobile_loginMember(checkMember, res);
   }
});

function loginMember(checkMember,req,res)
{
   var inputMemberEmail = checkMember.inputMemberEmail;
   var inputMemberPassword = checkMember.inputMemberPassword;

   MemberDAO.checkLoginMember(inputMemberEmail,inputMemberPassword,function(data)
   {
      if(data==true) {

         MemberDAO.getIndexMember(inputMemberEmail,function(data)
         {
            if(data!=false)
            {
               req.session.MemberEmail = inputMemberEmail;

               res.status(200).send(true);
            }
         });
      }
      else
         res.status(200).send(false);
   });
}

function joinMember(checkMember,res)
{
   var inputRegisterEmail = checkMember.inputRegisterEmail;
   var inputRegisterPassword = checkMember.inputRegisterPassword;
   var inputRegisterFirstname = checkMember.inputRegisterFirstname;
   var inputRegisterLastname = checkMember.inputRegisterLastname;
   var inputIndex;

   MemberDAO.initIndexers();

   MemberDAO.getIndexers("user_index",function(data)
   {
      inputIndex = data;
   });


   MemberDAO.checkIdMember(inputRegisterEmail
       ,function(data){
      if(data==true)
         res.status(200).send(false);
      else
      {
         MemberDAO.insertMember(inputIndex,inputRegisterEmail,inputRegisterPassword,inputRegisterFirstname,inputRegisterLastname);

         res.status(200).send(true);
      }
   });
}

function idCheckMember(checkMember,res)
{
   var inputRegisterEmail = checkMember.inputRegisterEmail;

   MemberDAO.checkIdMember(inputRegisterEmail,function(data){
      if(data==true)
         res.status(200).send(true);
      else
         res.status(200).send(false);
   });
}


function mobile_loginMember(checkMember,res)
{
   var inputMemberEmail = checkMember.inputMemberEmail;
   var inputMemberPassword = checkMember.inputMemberPassword;

   MemberDAO.checkLoginMember(inputMemberEmail,inputMemberPassword,function(data)
   {
      if(data==true) {

         MemberDAO.getIndexMember(inputMemberEmail,function(data)
         {
            if(data!=false)
            {
               var json = JSON.stringify({
                  login_result : "true"
               });

               res.end(json);
            }
         });
      }
      else {
         var json = JSON.stringify({
            login_result : "false"
         });

         res.end(json);
       }
   });
}



module.exports = router;
