'use strict'

const jwt = require('jsonwebtoken');

const verify_json_web_token = (req, res, next) => {

    const authorization_header = req.headers.authorization || req.headers.Authorization;

    // console.log("Request : ", req.headers);

    if (!(authorization_header && authorization_header.startsWith("Bearer ")))
        return res.status(401).json({ 'message': 'unauthorized user' });

    const received_token = authorization_header.split(' ')[1];

    jwt.verify(
        received_token,
        req.config.process_env.ACCESS_TOKEN_SECRET,
        function (error, decoded_data) {

            if (error) {
                console.log('JW token vefication error : ', error);
                return res.status(403).json({ 'message': 'Resource access forbidden' });
            }

            console.log(decoded_data);
            req.user = decoded_data.user_info.username;
            req.role = decoded_data.user_info.role;
            next();
        }
    );
}

module.exports = { verify_json_web_token };