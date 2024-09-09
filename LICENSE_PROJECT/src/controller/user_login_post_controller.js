'use strict'

const bcrypt = require('bcrypt');
const axios = require('axios');
const jwt = require('jsonwebtoken');
const home_controller = require('../controller/home_controller');

const handle_login = async (req, res) => {

    this.db_bucket = req.config.db_bucket;
    this.process_env = req.config.process_env;

    const collections = this.db_bucket.defaultCollection();

    try {
        const view_url = 'http://localhost:8092/' + this.db_bucket._name + '/_design/user_details_doc/_view/username_view?stale=false';

        const view_result = await axios.get(view_url, {
            auth: {
                username: this.process_env.DB_LICENSE_BUCKET_UNAME,
                password: this.process_env.DB_LICENSE_BUCKET_PASSWORD
            }
        });

        const { username, password } = req.body

        const user_credentials = view_result.data.rows.map((row) => {
            return { 'username': row.key, 'password': row.value.password, 'role': row.value.role };
        });

        var found_user = user_credentials.find(credential => credential.username === username);

        if (!found_user) {
            console.log("Username failure");
            return res.status(401).redirect('/login_view');
        }

        const password_match = await bcrypt.compare(password, found_user.password);

        if (!password_match) {
            console.log("Password failure");
            return res.status(401).redirect('/login_view');
        }

        const access_token = jwt.sign(
            {
                'user_info': {
                    'username': found_user.username,
                    'role': found_user.role
                }
            },
            this.process_env.ACCESS_TOKEN_SECRET,
            { expiresIn: "1d" }
        );

        const refresh_token = jwt.sign(
            { 'username': found_user.username },
            this.process_env.REFRESH_TOKEN_SECRET,
            { expiresIn: "1d" }
        );

        const user_info_document = await (collections.get(view_result.data.rows[0].id));

        var updated_user_info_document = {
            'users': []
        };

        user_info_document.content.users.forEach(user => {

            if (user.username === found_user.username) {
                user['refresh_token'] = refresh_token;
                console.log("Refresh token : ", refresh_token);
            }

            updated_user_info_document.users.push(user);
        });

        await collections.upsert(view_result.data.rows[0].id, updated_user_info_document);

        // Enable "secure:true" when using in production server.
        res.cookie('jw_token', refresh_token, { httpOnly: true, sameSite: 'None', /*secure: true*,*/ maxAge: 24 * 60 * 60 * 1000 })

        req.session.isAuth = true;
        req.session.access_token = access_token;
        // res.status(200).json({ 'access_token': access_token });
        console.log("redirecting to home");
        res.redirect('/home');

    } catch (error) {
        console.log(error.message);
    }
}

module.exports = { handle_login };