'use strict'

const axios = require('axios');
const { json } = require('express');
const jwt = require('jsonwebtoken');

const handle_refresh_token = async (req, res) => {

    this.db_bucket = req.config.db_bucket;
    this.process_env = req.config.process_env;

    const cookies = req.cookies;

    // console.log(cookies);
    if (!(cookies && cookies.jw_token)) {
        return res.status(401).send();
    }

    const received_refresh_token = cookies.jw_token;

    try {
        const view_url = 'http://localhost:8092/' + this.db_bucket._name + '/_design/user_details_doc/_view/refresh_token_view';

        const view_result = await axios.get(view_url, {
            auth: {
                username: this.process_env.DB_LICENSE_BUCKET_UNAME,
                password: this.process_env.DB_LICENSE_BUCKET_PASSWORD
            }
        });

        const user_refresh_token = view_result.data.rows.map((row) => {
            return { 'username': row.key, 'refresh_token': row.value.refresh_token, 'role': row.value.role };
        });

        var found_user = user_refresh_token.find(credential => credential.refresh_token === received_refresh_token);

        if (!found_user)
            return res.status(403).json({ 'message': 'Forbidden resource' });

        jwt.verify(received_refresh_token, req.config.process_env.REFRESH_TOKEN_SECRET, function (error, decoded_data) {

            if (error || found_user.username !== decoded_data.username) {
                console.log('JW refresh token vefication error : ', error);
                return res.status(403).json({ 'message': 'Resource access forbidden' });
            }

            // console.log(decoded_data);
            req.user = decoded_data.username;

            const access_token = jwt.sign(
                {
                    'user_info': {
                        'username': decoded_data.username,
                        'role': found_user.role
                    }
                },
                req.config.process_env.ACCESS_TOKEN_SECRET,
                { expiresIn: "30s" }
            );
            res.status(200).json({ 'access_token': access_token });
        });

    } catch (error) {
        console.log(error);
    }
}

module.exports = { handle_refresh_token };