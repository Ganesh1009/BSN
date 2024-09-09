'use strict';

const express = require('express');
const axios = require('axios')
var path = require("path");

const router = express.Router();


const handle_login_view = async (req, res, next) => {

    (async () => {
        console.log("Insider login view controller");
        res.render('login_view');
    })();
};

module.exports = { handle_login_view };