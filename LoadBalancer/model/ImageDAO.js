/**
 * Created by KaeulHan on 2016. 2. 16..
 */
var db = require('../util/DBUtil');
var Images = db.getImagesConnection();
var ImageIndexers = db.getImageIndexersConnection();
var ImageListDO = require('../dataobject/ImageListDO');

exports.initIndexers = function()
{
    ImageIndexers.insert(
        {
            _id: "image_index",
            seq: 0
        });
}



exports.getIndexers = function(target,callback)
{
    ImageIndexers.findAndModify(
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

exports.checkImageList = function(inputRegisterEmail, callback) {

    var ImageList = [];

    Images.find({Email: inputRegisterEmail}).toArray(function (err, result) {

        if(err)
        {
           console.log(err.message);
        }
        else if (result != null) {


            for (var i = 0; i < result.length; i++) {
                //console.log(result[i].Index+" "+result[i].Image);
                var ImageInfo = new ImageListDO.ImageListDO(result[i].Index, result[i].Email, result[i].Image);

               // console.log(result[i].Index+" "+result[i].Image);

                ImageList.push(ImageInfo);
                //console.log(ImageList[i]);
            }

            callback(ImageList);
        }
        else {
            callback(false);
        }
    });



}

exports.insertImage = function(inputIndex, inputRegisterEmail, inputImageVersion)
{
    Images.insert({
        Index : inputIndex,
        Email : inputRegisterEmail,
        Image : inputImageVersion
    },function(err,result)
    {
        if(err)
        {
            throw error;
        }
    });
}

exports.deleteImage = function(inputIndex, inputRegisterEmail, inputImageVersion, callback)
{

    console.log(inputIndex);

    Images.remove({
        Index : parseInt(inputIndex),
       Email : inputRegisterEmail,
       Image : inputImageVersion
    },function(err,result)
    {
    //    console.log(result);

        if(err)
        {
            console.error(error);
      //      throw error;
        }

        callback(result);

    });


}

