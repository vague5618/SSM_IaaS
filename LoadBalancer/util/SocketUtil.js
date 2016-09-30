/**
 * Created by jay on 16. 2. 11.
 */

exports.getSocketStart = function(targetIP,command, ostype, index, id, ip, connectType, imagePort, audioPort, eventPort)
{
    var net = require('net');

    var socket_client = new net.Socket();

    socket_client.connect(5799, targetIP, function() {
        console.log('Connected Socket');
        socket_client.write(command.toString()+" "+ostype.toString()+" "+index.toString()+" "+id.toString()+" "+
        ip.toString()+" "+connectType.toString()+" "+imagePort.toString()+" "+audioPort.toString()+" "+eventPort.toString()+" ");
    });

    socket_client.on('close', function() {
        console.log('Connection closed');
    });

    socket_client.on('error', function() {
        console.log('Error! connect socket');
    });
}

exports.getSocketCreate = function(targetIP, command, ostype, index, id)
{
    var net = require('net');

    var socket_client = new net.Socket();

    socket_client.connect(5799, targetIP, function() {
        console.log('Connected Socket');
        socket_client.write(command.toString()+" "+ostype.toString()+" "+index.toString()+" "+id.toString()+" ");
    });

    socket_client.on('close', function() {
        console.log('Connection closed');
    });

    socket_client.on('error', function() {
        console.log('Error! connect socket');
    });
}

exports.getSocketDelete = function(targetIP, command, ostype, index, id)
{
    var net = require('net');

    var socket_client = new net.Socket();

    socket_client.connect(5799, targetIP, function() {
        console.log('Connected Socket');
        socket_client.write(command.toString()+" "+ostype.toString()+" "+index.toString()+" "+id.toString()+" ");
    });

    socket_client.on('close', function() {
        console.log('Connection closed');
    });

    socket_client.on('error', function() {
        console.log('Error! connect socket');
    });
}

exports.getSocketInstall = function(targetIP, command, ostype, index, id, ip, connectType, imagePort, audioPort, eventPort)
{
    var net = require('net');

    var socket_client = new net.Socket();

    socket_client.connect(5799, targetIP, function() {
        console.log('Connected Socket');
        socket_client.write(command.toString()+" "+ostype.toString()+" "+index.toString()+" "+id.toString()+" "+
            ip.toString()+" "+connectType.toString()+" "+imagePort.toString()+" "+audioPort.toString()+" "+eventPort.toString()+" ");
    });

    socket_client.on('close', function() {
        console.log('Connection closed');
    });

    socket_client.on('error', function() {
        console.log('Error! connect socket');
    });
}
