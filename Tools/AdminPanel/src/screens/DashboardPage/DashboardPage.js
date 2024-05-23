import './DashboardPage.scss';
import React from "react";
import Navbar from "../../components/Navbar/Navbar";
import Sidebar from "../../components/Sidebar/Sidebar";
import LocalSessionServices from "./LocalSessionServices/LocalSessionServices";
import LocalDirectoryServices from "./LocalDirectoryServices/LocalDirectoryServices";
import hubIcon from "../../resources/icons/hub-icon.png";

const Dashboard = () => {
    const localSessionServicesData = [
        {
            sessions: [
                { user: 'User1', accessedLDS: ['LDS #1', 'LDS #2'] },
                { user: 'User2', accessedLDS: ['LDS #1', 'LDS #3'] }
            ]
        },
        {
            sessions: [
                { user: 'User3', accessedLDS: ['LDS #1', 'LDS #2'] },
                { user: 'User4', accessedLDS: ['LDS #1', 'LDS #3'] },
                { user: 'User5', accessedLDS: ['LDS #2', 'LDS #3'] }
            ]
        }
    ];

    const localDirectoryServicesData = [
        {
            name: 'Local Directory Service #1',
            cpuUsage: '10%',
            memoryUsage: '20%',
            localServices: [
                { name: 'Service #1' },
                { name: 'Service #2' }
            ]
        },
        {
            name: 'Local Directory Service #2',
            cpuUsage: '15%',
            memoryUsage: '25%',
            localServices: [
                { name: 'Service #1' },
                { name: 'Service #2' }
            ]
        },
        {
            name: 'Local Directory Service #3',
            cpuUsage: '20%',
            memoryUsage: '30%',
            localServices: [
                { name: 'Service #1' },
                { name: 'Service #2' }
            ]
        }
    ];

    return (
        <div className="dashboardPage">
            <Sidebar items="dashboard" />
            <div className="listContainer">
                <Navbar />
                <h2>Dashboard</h2>
                <div className="dashboardContainer">
                    <div className="grid-item">
                        <LocalSessionServices services={localSessionServicesData} />
                    </div>
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
