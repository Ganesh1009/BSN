'use strict';

require('dotenv/config')

const express = require('express');
const app = express();
const path = require('path');
const PORT = 3000;

const DB_Connections = require('./utils/DB_Connections');

const home = require('./routes/home');
const modify = require('./routes/db_operations');
const user_registration = require('./routes/user_registration');
const user_login = require('./routes/user_login');
const user_logout = require('./routes/user_logout');
const token_verification = require('./utils/JW_Token_Verification');
const user_refresh_token = require('./routes/refresh_token');
const cookie_parser = require('cookie-parser');
const cors = require('cors');
const redis = require('redis');
const session = require('express-session');
const redis_store = require('connect-redis')(session);

//Configure redis client
const redis_client = redis.createClient({ legacyMode: true });
redis_client.connect().catch(console.error)

const cb_connection = new DB_Connections(process.env);

const session_store = new redis_store({
    host: 'localhost',
    port: 6379,
    client: redis_client
});

app.use(express.urlencoded({ extended: false }));
app.use(express.json());
app.use(cookie_parser());
app.use(cors({ origin: true, credentials: true }));
app.use(
    session({
        secret: 'Secret key to sign the cookie',
        resave: false, // If set to true, for every request there will be a new session.
        saveUninitialized: false,
        store: session_store,
        cookie: {
            httpOnly: true,
            sameSite: 'None',
            maxAge: 24 * 60 * 60 * 1000
        }
    })
);

app.set("utils", path.join(__dirname, 'utils'));
app.set("views", path.join(__dirname, "views"));
app.set("view engine", "ejs");

const bucket_promise = cb_connection.connect();

const isAuth = (req, res, next) => {
    // console.log("req session : ", req.session);
    if (req.session.isAuth)
        next();
    else
        res.redirect('/login');
};

bucket_promise.then(bucket => {

    const configuration = {
        db_bucket: bucket,
        process_env: process.env
    }

    app.get('/', function (req, res) {
        res.redirect('/login_view');
    });

    app.use('/login_view', function (req, res, next) {
        console.log('Login view request received');
        next();
    }, user_login);

    app.use('/login', function (req, res, next) {
        console.log('Login request received');
        req.config = configuration;
        next();
    }, user_login);

    app.use('/registration_view', function (req, res, next) {
        console.log('Registration request received');
        req.config = configuration;
        next();
    }, user_registration);

    app.use('/register', function (req, res, next) {
        req.config = configuration;
        next();
    }, user_registration);

    app.use('/refresh', function (req, res, next) {
        console.log("Refresh request received...");
        req.config = configuration;
        next();
    }, user_refresh_token);

    app.use('/logout', function (req, res, next) {
        console.log("Logout request received...");
        req.config = configuration;
        next();
    }, user_logout);


    // Token verification util(middleware).
    app.use(function (req, res, next) {
        req.config = configuration;
        req.headers.authorization = "Bearer " + req.session.access_token;
        console.log('Token verification :', req.headers);
        next();
    }, token_verification.verify_json_web_token);


    app.use('/home', isAuth, function (req, res, next) {
        console.log('Requesting home content');
        req.config = configuration;
        next();
    }, home);

    app.use('/modify', isAuth, function (req, res, next) {
        req.config = configuration;
        next();
    }, modify);
});

// Starting node server and listening on port 3000
app.listen(PORT, error => {

    if (error) {
        return console.log("Failed to start application : ", error);
    }

    console.log(`listening on port ${PORT}`);
});