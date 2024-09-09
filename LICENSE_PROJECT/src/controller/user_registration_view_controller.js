'use strict';

const express = require('express');
const axios = require('axios')
var path = require("path");

const router = express.Router();


const handle_registration_view = async (req, res, next) => {

    (async () => {
        console.log("Insider login view controller");
        res.render('registration_view');
    })();
};

module.exports = { handle_registration_view };