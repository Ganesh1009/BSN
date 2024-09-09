'use strict'

const express = require('express');
const router = express.Router();
const login_controller = require('../controller/user_login_post_controller');
const login_view_controller = require('../controller/user_login_view_controller');

router.get('/', login_view_controller.handle_login_view);

router.post('/', login_controller.handle_login);

module.exports = router;