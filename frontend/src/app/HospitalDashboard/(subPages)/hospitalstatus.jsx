'use client';

import { useState } from 'react';
import { Card, Form, Button, Row, Col, Alert, Badge } from 'react-bootstrap';

const initialStatus = {
  beds: {
    total: 120,
    available: 25,
    icu: 15,
    emergency: 10,
  },
  ventilation: {
    ventilators: 6,
    oxygenUnits: 52,
  },
  medicineStock: 'Medium', // High, Medium, Low
  staff: {
    doctors: 12,
    nurses: 22,
    emergencyTeam: 6,
  },
  equipment: {
    xrayMachines: 2,
    ecgMachines: 3,
    backupPower: true,
  },
  sanitation: {
    isolationRooms: 4,
    lastCleaned: '2025-06-18',
  },
};

export default function HospitalStatus() {
  const [status, setStatus] = useState(initialStatus);
  const [form, setForm] = useState(initialStatus);
  const [updated, setUpdated] = useState(false);

  const handleChange = (e, section, field) => {
    const value = e.target.type === 'checkbox' ? e.target.checked : e.target.value;
    const parsed = !isNaN(value) && e.target.type !== 'checkbox' ? parseInt(value) : value;

    setForm({
      ...form,
      [section]: {
        ...form[section],
        [field]: parsed,
      },
    });
  };

  const handleSubmit = (e) => {
    e.preventDefault();
    setStatus(form);
    setUpdated(true);
    setTimeout(() => setUpdated(false), 2000);
  };

  return (
    <div className="container-fluid py-4">
      {/* Header */}
      <div className="d-flex justify-content-between align-items-center mb-4">
        <div>
          <h4 className="fw-bold text-primary mb-1">
            <i className="bi bi-hospital me-2"></i>Hospital Resource Dashboard
          </h4>
          <p className="text-muted small mb-0">
            Track and update your hospital's real-time infrastructure and medical capacity.
          </p>
        </div>
      </div>

      <Card className="shadow-sm">
        <Card.Body>
          {updated && <Alert variant="success">Hospital status updated ✅</Alert>}
          <Form onSubmit={handleSubmit}>
            {/* BED STATUS */}
            <h5 className="fw-semibold text-dark mt-2">🛏️ Beds</h5>
            <Row className="mb-3">
              <Col md={3}>
                <Form.Label>Total Beds</Form.Label>
                <Form.Control
                  type="number"
                  value={form.beds.total}
                  onChange={(e) => handleChange(e, 'beds', 'total')}
                />
              </Col>
              <Col md={3}>
                <Form.Label>Available Beds</Form.Label>
                <Form.Control
                  type="number"
                  value={form.beds.available}
                  onChange={(e) => handleChange(e, 'beds', 'available')}
                />
              </Col>
              <Col md={3}>
                <Form.Label>ICU Beds</Form.Label>
                <Form.Control
                  type="number"
                  value={form.beds.icu}
                  onChange={(e) => handleChange(e, 'beds', 'icu')}
                />
              </Col>
              <Col md={3}>
                <Form.Label>Emergency Beds</Form.Label>
                <Form.Control
                  type="number"
                  value={form.beds.emergency}
                  onChange={(e) => handleChange(e, 'beds', 'emergency')}
                />
              </Col>
            </Row>

            {/* VENTILATION & OXYGEN */}
            <h5 className="fw-semibold text-dark mt-3">💨 Ventilation</h5>
            <Row className="mb-3">
              <Col md={4}>
                <Form.Label>Ventilators</Form.Label>
                <Form.Control
                  type="number"
                  value={form.ventilation.ventilators}
                  onChange={(e) => handleChange(e, 'ventilation', 'ventilators')}
                />
              </Col>
              <Col md={4}>
                <Form.Label>Oxygen Units</Form.Label>
                <Form.Control
                  type="number"
                  value={form.ventilation.oxygenUnits}
                  onChange={(e) => handleChange(e, 'ventilation', 'oxygenUnits')}
                />
              </Col>
            </Row>

            {/* MEDICINE STOCK */}
            <h5 className="fw-semibold text-dark mt-3">💊 Medicine Stock</h5>
            <Row className="mb-3">
              <Col md={4}>
                <Form.Label>Stock Level</Form.Label>
                <Form.Select
                  value={form.medicineStock}
                  onChange={(e) => setForm({ ...form, medicineStock: e.target.value })}
                >
                  <option value="High">High</option>
                  <option value="Medium">Medium</option>
                  <option value="Low">Low</option>
                </Form.Select>
              </Col>
            </Row>

            {/* STAFF */}
            <h5 className="fw-semibold text-dark mt-3">🧑‍⚕️ Staff On-Duty</h5>
            <Row className="mb-3">
              <Col md={3}>
                <Form.Label>Doctors</Form.Label>
                <Form.Control
                  type="number"
                  value={form.staff.doctors}
                  onChange={(e) => handleChange(e, 'staff', 'doctors')}
                />
              </Col>
              <Col md={3}>
                <Form.Label>Nurses</Form.Label>
                <Form.Control
                  type="number"
                  value={form.staff.nurses}
                  onChange={(e) => handleChange(e, 'staff', 'nurses')}
                />
              </Col>
              <Col md={3}>
                <Form.Label>Emergency Team</Form.Label>
                <Form.Control
                  type="number"
                  value={form.staff.emergencyTeam}
                  onChange={(e) => handleChange(e, 'staff', 'emergencyTeam')}
                />
              </Col>
            </Row>

            {/* EQUIPMENT */}
            <h5 className="fw-semibold text-dark mt-3">🧪 Equipment</h5>
            <Row className="mb-3">
              <Col md={3}>
                <Form.Label>X-Ray Machines</Form.Label>
                <Form.Control
                  type="number"
                  value={form.equipment.xrayMachines}
                  onChange={(e) => handleChange(e, 'equipment', 'xrayMachines')}
                />
              </Col>
              <Col md={3}>
                <Form.Label>ECG Machines</Form.Label>
                <Form.Control
                  type="number"
                  value={form.equipment.ecgMachines}
                  onChange={(e) => handleChange(e, 'equipment', 'ecgMachines')}
                />
              </Col>
              <Col md={3} className="d-flex align-items-end">
                <Form.Check
                  type="checkbox"
                  label="Backup Power Available"
                  checked={form.equipment.backupPower}
                  onChange={(e) => handleChange(e, 'equipment', 'backupPower')}
                />
              </Col>
            </Row>

            {/* SANITATION */}
            <h5 className="fw-semibold text-dark mt-3">🧼 Sanitation</h5>
            <Row className="mb-4">
              <Col md={3}>
                <Form.Label>Isolation Rooms</Form.Label>
                <Form.Control
                  type="number"
                  value={form.sanitation.isolationRooms}
                  onChange={(e) => handleChange(e, 'sanitation', 'isolationRooms')}
                />
              </Col>
              <Col md={4}>
                <Form.Label>Last Cleaned Date</Form.Label>
                <Form.Control
                  type="date"
                  value={form.sanitation.lastCleaned}
                  onChange={(e) => handleChange(e, 'sanitation', 'lastCleaned')}
                />
              </Col>
            </Row>

            <Button variant="primary" type="submit">
              <i className="bi bi-save me-1"></i> Update Status
            </Button>
          </Form>
        </Card.Body>
      </Card>
    </div>
  );
}
