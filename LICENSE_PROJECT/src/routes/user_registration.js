'use strict'

const express = require('express');
const router = express.Router();

const user_registration_controller = require('../controller/user_registration_controller');
const registration_view_controller = require('../controller/user_registration_view_controller');

router.get('/', registration_view_controller.handle_registration_view);
router.post('/', user_registration_controller.handle_registration);

module.exports = router;