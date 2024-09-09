'use strict'

const express = require('express');
const router = express.Router();
const refresh_roken_controller = require('../controller/refresh_token_controller');

router.get('/', refresh_roken_controller.handle_refresh_token);

module.exports = router;