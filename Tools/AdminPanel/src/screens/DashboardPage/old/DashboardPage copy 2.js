import './DashboardPage.scss';
import React, { useState, useEffect } from "react";
import Navbar from "../../../components/Navbar/Navbar";
import Sidebar from "../../../components/Sidebar/Sidebar";
import DataProvider from '../DataProvider';
import Modal from 'react-modal';  // Fügen Sie react-modal hinzu: npm install react-modal

Modal.setAppElement('#root');  // Stellen Sie sicher, dass das Modal korrekt gerendert wird

const Dashboard = () => {
    const [sessionData, setSessionData] = useState(null);
    const [localSessionServices, setLocalSessionServices] = useState([]);
    const [localDirectoryServices, setLocalDirectoryServices] = useState([]);
    const [loading, setLoading] = useState(true);
    const [modalIsOpen, setModalIsOpen] = useState(false);
    const [modalContent, setModalContent] = useState(null);
    const [serviceDetails, setServiceDetails] = useState(null);

    useEffect(() => {
        const fetchData = async () => {
            const dataProvider = new DataProvider();
            await dataProvider.initialize();
            const localSessionServiceURL = dataProvider.globalSessionService["LocalSessionServices"][0]["URL"];
            const sessionResponse = await dataProvider.getInformation(localSessionServiceURL);

            setSessionData(dataProvider.globalSessionService);
            setLocalSessionServices(dataProvider.getLocalSessionServices());
            setLocalDirectoryServices(dataProvider.getLocalDirectoryServices());
            setLoading(false);
        };

        fetchData();
    }, []);

    const filterRelayService = (data) => {
        if (data && data["Session.List"]) {
            data["Session.List"] = data["Session.List"].map(session => {
                session["Session.Services"] = session["Session.Services"].filter(service => service["Service.Type"] !== "RelayService");
                return session;
            });
        }
        return data;
    };

    const openModal = async (url) => {
        const dataProvider = new DataProvider();
        let sessionData = await dataProvider.getInformation(url);
        sessionData = filterRelayService(sessionData);  // Filter the RelayService
        setModalContent(sessionData);
        setModalIsOpen(true);
    };

    const closeModal = () => {
        setModalIsOpen(false);
        setModalContent(null);
        setServiceDetails(null);
    };

    const fetchServiceDetails = async (url) => {
        const dataProvider = new DataProvider();
        const details = await dataProvider.getInformation(url);
        setServiceDetails(details);
    };

    if (loading) {
        return <div>Loading...</div>;
    }

    return (
        <div className="dashboardPage">
            <Sidebar items="dashboard" />
            <div className="listContainer">
                <Navbar />
                <h2>Dashboard</h2>
                <div className="dashboardContainer">
                    <div className="grid-item">
                        <h3>Local Session Services</h3>
                        <ul>
                            {localSessionServices.map((service, index) => (
                                <li key={index}>
                                    {service.URL}
                                    <button onClick={() => openModal(service.URL)}>Show Details</button>
                                </li>
                            ))}
                        </ul>
                    </div>
                    <div className="grid-item">
                        <h3>Local Directory Services</h3>
                        <ul>
                            {localDirectoryServices.map((service, index) => (
                                <li key={index}>
                                    {service.URL}
                                    <button onClick={() => openModal(service.URL)}>Show Details</button>
                                </li>
                            ))}
                        </ul>
                    </div>
                </div>
            </div>

            <Modal
                isOpen={modalIsOpen}
                onRequestClose={closeModal}
                contentLabel="Session Details"
            >
                <h2>Session Details</h2>
                <button onClick={closeModal}>Close</button>
                <pre>{JSON.stringify(modalContent, null, 2)}</pre>
                {modalContent && modalContent["Session.List"] && modalContent["Session.List"].map((session, index) => (
                    <div key={index}>
                        <h3>Session ID: {session["Session.ID"]}</h3>
                        <ul>
                            {session["Session.Services"].map((service, idx) => (
                                <li key={idx}>
                                    {service["Service.URL"]} - {service["Service.Type"]}
                                    <button onClick={() => fetchServiceDetails(service["Service.URL"])}>Get Service Details</button>
                                </li>
                            ))}
                        </ul>
                    </div>
                ))}
                {serviceDetails && (
                    <div>
                        <h3>Service Details</h3>
                        <pre>{JSON.stringify(serviceDetails, null, 2)}</pre>
                    </div>
                )}
            </Modal>
        </div>
    );
};

export default Dashboard;
