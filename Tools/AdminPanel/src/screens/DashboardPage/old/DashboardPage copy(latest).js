import './DashboardPage.scss';
import React, { useState, useEffect } from "react";
import Navbar from "../../../components/Navbar/Navbar";
import DataProvider from '../DataProvider';
import SessionServices from './SessionServices';
import LocalSessionServiceModal from './LocalSessionServiceModal';

const Dashboard = () => {
    const [dataProvider, setDataProvider] = useState(null);
    const [loading, setLoading] = useState(true);
    const [modalIsOpen, setModalIsOpen] = useState(false);
    const [modalContent, setModalContent] = useState(null);
    const [serviceDetails, setServiceDetails] = useState(null);

    useEffect(() => {
        const fetchData = async () => {
            const provider = new DataProvider();
            await provider.initialize();
            setDataProvider(provider);
            setLoading(false);
        };

        fetchData();
    }, []);

    const filterRelayService = (data) => {
        if (data && data["Session.List"]) {
            data["Session.List"] = data["Session.List"].map(session => {
                session["Session.Services"] = session["Session.Services"].filter(service => service["Service.Type"] !== "RelayService" && service["Service.Type"] !== "UI");
                return session;
            });
        }
        return data;
    };

    const openModal = async (url) => {
        const provider = new DataProvider();
        let sessionData = await provider.getInformation(url);
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
        const provider = new DataProvider();
        const details = await provider.getInformation(url);
        setServiceDetails(details);
    };

    if (loading) {
        return <div>Loading...</div>;
    }

    return (
        <div className="dashboardPage">
            <div className="listContainer">
                <Navbar />
                <h2>Dashboard</h2>
                <div className="dashboardContainer">
                    <SessionServices
                        title="Global Session Service"
                        globalSessionService={dataProvider}
                        openModal={openModal}
                    />
                </div>
            </div>

            <LocalSessionServiceModal
                isOpen={modalIsOpen}
                onRequestClose={closeModal}
                modalContent={modalContent}
                fetchServiceDetails={fetchServiceDetails}
                serviceDetails={serviceDetails}
            />
        </div>
    );
};

export default Dashboard;
