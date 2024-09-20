/**
 * Retrieves system information of a service via the server the auth-server.
 * @param {string} serviceURL - The URL of the service.
 * @returns {Promise} - A promise that resolves with the global session information.
 */
import authRequest from "../FesRequest/authRequest";


export function GetInformation(serviceURL) {
    const data = {
        action: "System.GetInformation",
        LoggedInUsername: "admin",
        LoggedInUserPassword: "admin",
    };

    data["Service.URL"] = serviceURL;

    return authRequest(data);
}

export default GetInformation;