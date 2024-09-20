import './DashboardPage.scss';
import React from "react";
import Navbar from "../../../components/Navbar/Navbar";
import Sidebar from "../../../components/Sidebar/Sidebar";
import LocalSessionServices from "./LocalSessionServices/LocalSessionServices";
import LocalDirectoryServices from "./LocalDirectoryServices/LocalDirectoryServices";
import hubIcon from "../../resources/icons/hub-icon.png";

import GetInformation from '../../../services/api/DashboardServices/GetInformationService';
import GetGlobalServices from '../../../services/api/DashboardServices/GetGlobalServices';
import { useEffect } from "react";

const Dashboard = () => {
    GetInformation("127.0.0.1:8093").then((response) => {
        console.log("--- GetGlobalSessionService ---");
        console.log(response["Session.List"]); // response.data sollte die eigentlichen Daten enthalten
        console.log("response received");
    }).catch((error) => {
        console.error("Error fetching data:", error);
    });
    GetGlobalServices("127.0.0.1:8091").then((response) => {
        console.log("--- GetGlobalServices ---");
        console.log(response); // response.data sollte die eigentlichen Daten enthalten
        console.log("response received");
    }).catch((error) => {
        console.error("Error fetching data:", error);
    });

    // console.log(GetGlobalSessionService());
    function generateLocalDirectoryServices(numServices) {
        const localDirectoryServicesData = [];
        const spec = 100 / numServices;

        for (let i = 1; i <= numServices; i++) {
            const localDirectoryService = {
                name: `Local Directory Service #${i}`,
                cpuUsage: `${spec}%`,
                memoryUsage: `${spec}%`,
                localSessionServices: []
            };

            const numSessionServices = Math.floor(Math.random() * 3) + 1; // Zufällige Anzahl von Session Services (1 bis 3)

            for (let j = 1; j <= numSessionServices; j++) {
                const sessionService = {
                    id: i * 1000 + j, // Eindeutige ID für jeden Service
                    name: `Service #${i * 1000 + j}`,
                    cpuUsage: `${spec}%`,
                    memoryUsage: `${spec}%`
                };
                localDirectoryService.localSessionServices.push(sessionService);
            }

            localDirectoryServicesData.push(localDirectoryService);
        }

        return localDirectoryServicesData;
    }

    const localDirectoryServicesData = generateLocalDirectoryServices(100);

    return (
        <div className="dashboardPage">
            <Sidebar items="dashboard" />
            <div className="listContainer">
                <Navbar />
                <h2>Dashboard</h2>
                <div className="dashboardContainer">
                    <div className="grid-item">
                        <LocalDirectoryServices localDirectoryServices={localDirectoryServicesData} />
                    </div>
                    <div className="grid-item">
                        <img className="gss-icon" src={hubIcon} alt="Hub Icon" />
                    </div>
                </div>
            </div>
        </div>
    );
};

export default Dashboard;
