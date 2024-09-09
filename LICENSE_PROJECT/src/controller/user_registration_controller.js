'use strict'

const bcrypt = require('bcrypt');
const axios = require('axios')

const handle_registration = async (req, res) => {

    const saltRounds = 10;

    this.db_bucket = req.config.db_bucket;
    this.process_env = req.config.process_env;

    const collection = this.db_bucket.defaultCollection();

    const { username, password } = req.body;

    var role = req.body.role;

    if (!username || !password)
        return res.status(400).json({ 'message': 'Username and password are mandatory' });

    /**
         * -----------------------------
         * PERMISSIONS FOR AUTHORIZATION
         * -----------------------------
         * 4 : Read-only
         * 2 : Write-only
         * 1 : execute-only
         * 
         * 3 : Write and execute
         * 5 : Read and execute
         * 6 : Read and write
         * 7 : Read, write and execute
    */
    const default_role = '4';

    if (!role)
        role = default_role;

    try {

        const salt = bcrypt.genSaltSync(saltRounds);

        const hashedPassword = await bcrypt.hash(password, salt);

        var user = {
            username: "",
            password: "",
            role: ""
        };

        user.username = username;
        user.password = hashedPassword;
        user.role = role;

        const view_url = 'http://localhost:8092/' + this.db_bucket._name + '/_design/user_details_doc/_view/username_view?stale=false';

        console.log(view_url);

        const view_result = await axios.get(view_url, {
            auth: {
                username: this.process_env.DB_LICENSE_BUCKET_UNAME,
                password: this.process_env.DB_LICENSE_BUCKET_PASSWORD
            }
        });

        const existing_usernames = view_result.data.rows.map((row) => {
            return row.key;
        });

        var db_operation_result = {};
        var users_json = {
            users: []
        };

        if (existing_usernames.length === 0) {

            users_json.users.push(user);
            // console.log(users_array);
            db_operation_result = await collection.upsert("user_infos", users_json);
            return res.status(201).redirect('/home');
        }

        const user_info_document = await collection.get(view_result.data.rows[0].id);

        users_json.users = user_info_document.content.users;
        console.log(user_info_document.content);
        const user_info_document_id = view_result.data.rows[0].id;

        if (existing_usernames.includes(username))
            return res.status(409).redirect('/login');

        users_json.users.push(user);

        db_operation_result = await collection.upsert(user_info_document_id, users_json);

        console.log(db_operation_result);

        res.status(201).redirect('/login');

    } catch (error) {
        console.log(`Error: ${error.message}`);
        res.status(500).json({ 'message': error.message });
    }
}

module.exports = { handle_registration };