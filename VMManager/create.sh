#!/bin/bash

ID=$1
OS=$2

mkdir ~/PROJECT/$ID
cp ~/PROJECT/baseImage/$OS ~/PROJECT/$ID/$OS
