/**
 * Created by ssm-jay on 16. 2. 22.
 */

var express = require('express');

var router = express.Router();

router.post('/', function(req, res, next) {

    var req_data = JSON.stringify(req.body);
    var req_Obj = JSON.parse(req_data);
    var checkTarget = req_Obj.action_target;

    console.log(checkTarget);

    if(checkTarget=="requestPort")
        requestPort(req,res);
});


function requestPort(req,res)
{
    console.log("port = " + req.session.port);

    res.send((req.session.port).toString());
}

module.exports = router;
