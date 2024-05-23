import './DashboardPage.scss';
import './DashboardPage.scss';

import React from "react";

import Navbar from "../../components/Navbar/Navbar";
import Sidebar from "../../components/Sidebar/Sidebar";

import hubIcon from "../../resources/icons/hub-icon.png";

const Dashboard = () => {
    console.log("Dashboard");
    return (
        <div class="dashboardPage" >
            <Sidebar items="dashboard" />
            <div class="listContainer">
                <Navbar />
                <h2>Dashboard</h2>
                <div class="dashboardContainer">
                    {/* <div class="grid-item">
                        <div class="card">
                            <div class="card-header">
                                Global Session Service
                            </div>
                            <div class="card-body">
                                <ul>
                                    <li>Status: online <div class="status-indicator"></div></li>
                                    <li>Address: 127.0.0.1</li>
                                </ul>
                            </div>
                        </div>
                        <div class="card">
                            <div class="card-header">
                                Global Direcotry Service
                            </div>
                            <div class="card-body">
                                <ul>
                                    <li>Status: online <div class="status-indicator"></div></li>
                                    <li>Address: 127.0.0.1</li>
                                </ul>
                            </div>
                        </div>
                    </div> */}
                    <div class="grid-item">
                        <div class="card">
                            <div class="card-header">
                                Local Session Services
                            </div>
                            <div class="card-body">
                                <div class="card">
                                    <div class="card-header">
                                        Local Session Service #1
                                    </div>
                                    <div class="card-body">
                                        <div class="inner-gird-container">
                                            <div class="card">
                                                <div class="card-header">
                                                    Session #1
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>User: User1</li>
                                                        <li>Accessed LDS:
                                                            <ul>
                                                                <li>LDS #1</li>
                                                                <li>LDS #2</li>
                                                            </ul>
                                                        </li>
                                                    </ul>
                                                </div>
                                            </div>
                                            <div class="card">
                                                <div class="card-header">
                                                    Session #2
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>User: User2</li>
                                                        <li>Accessed LDS:
                                                            <ul>
                                                                <li>LDS #1</li>
                                                                <li>LDS #3</li>
                                                            </ul>
                                                        </li>
                                                    </ul>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                                <div class="card">
                                    <div class="card-header">
                                        Local Session Service #2
                                    </div>
                                    <div class="card-body">
                                        <div class="inner-gird-container">
                                            <div class="card">
                                                <div class="card-header">
                                                    Session #1
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>User: User3</li>
                                                        <li>Accessed LDS:
                                                            <ul>
                                                                <li>LDS #1</li>
                                                                <li>LDS #2</li>
                                                            </ul>
                                                        </li>
                                                    </ul>
                                                </div>
                                            </div>
                                            <div class="card">
                                                <div class="card-header">
                                                    Session #2
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>User: User4</li>
                                                        <li>Accessed LDS:
                                                            <ul>
                                                                <li>LDS #1</li>
                                                                <li>LDS #3</li>
                                                            </ul>
                                                        </li>
                                                    </ul>
                                                </div>
                                            </div>
                                            <div class="card">
                                                <div class="card-header">
                                                    Session #3
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>User: User5</li>
                                                        <li>Accessed LDS:
                                                            <ul>
                                                                <li>LDS #2</li>
                                                                <li>LDS #3</li>
                                                            </ul>
                                                        </li>
                                                    </ul>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="grid-item">
                        <div class="card">
                            <div class="card-header">
                                Local Directory Services
                            </div>
                            <div class="card-body">
                                <div class="card">
                                    <div class="card-header">
                                        Local Directory Service #1
                                        <ul>
                                            <li style={{ listStyleType: "none" }}>CPU usage: 0%</li>
                                            <li style={{ listStyleType: "none" }}>Memory usage: 0%</li>
                                        </ul>
                                    </div>
                                    <div class="card-body">
                                        <div class="inner-gird-container">
                                            <div class="card">
                                                <div class="card-header">
                                                    Service #1
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>Name: Service#1</li>
                                                    </ul>
                                                </div>
                                            </div>
                                            <div class="card">
                                                <div class="card-header">
                                                    Service #2
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>Name: Service#2</li>
                                                    </ul>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                                <div class="card">
                                    <div class="card-header">
                                        Local Directory Service #2
                                    </div>
                                    <div class="card-body">
                                        <div class="inner-gird-container">
                                            <div class="card">
                                                <div class="card-header">
                                                    Service #1
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>Name: -</li>
                                                        <li>CPU usage: 0%</li>
                                                        <li>Memory usage: 0%</li>
                                                    </ul>
                                                </div>
                                            </div>
                                            <div class="card">
                                                <div class="card-header">
                                                    Service #2
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>Name: -</li>
                                                        <li>CPU usage: 0%</li>
                                                        <li>Memory usage: 0%</li>
                                                    </ul>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                                <div class="card">
                                    <div class="card-header">
                                        Local Directory Service #3
                                    </div>
                                    <div class="card-body">
                                        <div class="inner-gird-container">
                                            <div class="card">
                                                <div class="card-header">
                                                    Service #1
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>Name: -</li>
                                                        <li>CPU usage: 0%</li>
                                                        <li>Memory usage: 0%</li>
                                                    </ul>
                                                </div>
                                            </div>
                                            <div class="card">
                                                <div class="card-header">
                                                    Service #2
                                                </div>
                                                <div class="card-body">
                                                    <ul>
                                                        <li>Name: -</li>
                                                        <li>CPU usage: 0%</li>
                                                        <li>Memory usage: 0%</li>
                                                    </ul>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="grid-item">
                        <img class="gss-icon" src={hubIcon} alt="Hub Icon" />
                    </div>
                </div>
            </div>
        </div >
    );
};

export default Dashboard;
