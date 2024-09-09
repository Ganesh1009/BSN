'use strict';

const express = require('express');
const home_controller = require('../controller/home_controller')
const router = express.Router();

router.get('/', home_controller.handle_home);

module.exports = router;