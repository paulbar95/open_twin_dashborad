import authRequest from "../FesRequest/authRequest";
import msRequest from "../FesRequest/msRequest";

export function GetGlobalSessionService(serviceURL) {
    const data = {
        action: "Get.GlobalServicesURL",
        LoggedInUsername: "admin",
        LoggedInUserPassword: "admin",
    };

    data["Service.URL"] = serviceURL;

    const authServiceAddress = "https://" + serviceURL + "/execute-one-way-tls";
    console.log("GSS URL: ", authServiceAddress);
    return msRequest(authServiceAddress, data)
}

export default GetGlobalSessionService;