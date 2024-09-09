'use strict'

const axios = require('axios');

const handle_logout = async (req, res) => {

    this.db_bucket = req.config.db_bucket;
    this.process_env = req.config.process_env;

    const collections = this.db_bucket.defaultCollection();

    const cookies = req.cookies;
    // console.log("Cookie information : ", req.cookies);

    if (!(cookies && cookies.jw_token)) {
        console.log("Logout without cookie information...");
        return res.redirect('/login_view');
    }

    const received_refresh_token = cookies.jw_token;

    try {
        const view_url = 'http://localhost:8092/' + this.db_bucket._name + '/_design/user_details_doc/_view/refresh_token_view?stale=false';

        const view_result = await axios.get(view_url, {
            auth: {
                username: this.process_env.DB_LICENSE_BUCKET_UNAME,
                password: this.process_env.DB_LICENSE_BUCKET_PASSWORD
            }
        });

        const users = view_result.data.rows.map((row) => {
            return { 'username': row.key, 'value': row.value };
        });
        // console.log(users);

        var found_user = users.find((credential) => credential.value.refresh_token === received_refresh_token);

        if (!found_user) {
            // Enable "secure:true" when using in production server.
            res.clearCookie('jw_token', { httpOnly: true, sameSite: 'None'/*, secure: true*/ })
            return res.status(204).json({ 'message': 'No content' });
        }

        // Delete refresh token from data base
        const user_info_document = await (collections.get(view_result.data.rows[0].id));
        // console.log(user_info_document);

        var updated_user_info_document = {
            'users': []
        };

        user_info_document.content.users.forEach(user => {
            if (user.username === found_user.username && user.refresh_token === found_user.value.refresh_token) {
                console.log("Deleting refresh token");
                delete user['refresh_token'];
            }
            updated_user_info_document.users.push(user);
        });

        await collections.upsert(view_result.data.rows[0].id, updated_user_info_document);
        req.session.destroy(function (err) {
            if (err) {
                console.log(err);
            } else {
                console.log("All sessions destroyed...");
                res.clearCookie('jw_token', { httpOnly: true, sameSite: 'None', secure: true });
                res.redirect('/login_view');
            }
        });


    } catch (error) {
        console.log(error);
    }
}

module.exports = { handle_logout };