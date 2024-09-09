'use strict'

const express = require('express')
const router = express.Router();
const db_controller = require('../controller/db_operations_controller');

router.post('/', db_controller.handle_db_operations);

module.exports = router;

