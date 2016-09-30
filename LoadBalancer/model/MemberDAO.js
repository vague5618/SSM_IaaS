
var db = require('../util/DBUtil');
var Members = db.getMembersConnection();
var Indexers = db.getMemberIndexersConnection();


exports.initIndexers = function()
{
    Indexers.insert(
        {
            _id: "user_index",
            seq: 0
        });
}


exports.getIndexers = function(target,callback)
{
    Indexers.findAndModify(
        {
            query: { _id: target },
            update: { $inc: { seq: 1 } },
            new: true
        },function(error, data)
        {
            if(error)
                throw error;
            else {
                callback(data.seq);
            }
        }
    );
}

exports.insertMember = function(inputIndex,inputRegisterEmail,inputRegisterPassword,inputRegisterFirstname,inputRegisterLastname)
{

    Members.insert({
        Index : inputIndex
        , Email : inputRegisterEmail
        , Password : inputRegisterPassword
        , Firstname :inputRegisterFirstname
        , Lastname :inputRegisterLastname

    },function(err,result)
    {
        if(err)
        {
            throw error;
        }
    });
}

exports.checkLoginMember = function(inputMemberEmail,inputMemberPassword, callback)
{
    Members.findOne({Email: inputMemberEmail, Password : inputMemberPassword},function(err,result)
    {
       if(result!=null) {
           callback(true);
       }
       else {
            callback(false);
       }
    });
}

exports.checkIdMember = function(inputRegisterEmail, callback)
{
    Members.findOne({Email: inputRegisterEmail},function(err,result)
    {
        if(result!=null) {
            callback(inputRegisterEmail);
        }
        else {
            callback(false);
        }
    });
}


exports.getIndexMember = function(inputRegisterEmail, callback)
{
    Members.findOne({Email: inputRegisterEmail},function(err,result)
    {
        if(result!=null) {
            callback(result.Index);
        }
        else {
            callback(false);
        }
    });
}

